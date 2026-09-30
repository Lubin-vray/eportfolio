<?php

namespace App\Controller;

use App\Entity\CvDownload;
use Doctrine\ORM\EntityManagerInterface;
use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\HttpFoundation\ResponseHeaderBag;
use Symfony\Component\Routing\Attribute\Route;

final class CvController extends AbstractController
{
    #[Route('/cv', name: 'app_cv', methods: ['GET', 'POST'])]
    public function index(
        Request $request,
        EntityManagerInterface $entityManager
    ): Response {
        if ($request->isMethod('POST')) {
            if (!$this->isCsrfTokenValid(
                'cv_download',
                (string) $request->request->get('_token')
            )) {
                throw $this->createAccessDeniedException('Jeton de sécurité invalide.');
            }

            $name = trim((string) $request->request->get('name'));
            $email = trim((string) $request->request->get('email'));

            if ($name === '' || !filter_var($email, FILTER_VALIDATE_EMAIL)) {
                $this->addFlash(
                    'error',
                    'Merci de saisir un nom et une adresse e-mail valide.'
                );

                return $this->redirectToRoute('app_cv');
            }

            $download = new CvDownload();
            $download->setFullName($name);
            $download->setEmail($email);

            $entityManager->persist($download);
            $entityManager->flush();

            $file = $this->getParameter('kernel.project_dir')
                . '/public/files/CV3.pdf';

            if (!file_exists($file)) {
                throw $this->createNotFoundException('Le fichier CV est introuvable.');
            }

            return $this->file(
                $file,
                'CV-Lubin-Vray.pdf',
                ResponseHeaderBag::DISPOSITION_ATTACHMENT
            );
        }

        return $this->render('cv/index.html.twig');
    }
}