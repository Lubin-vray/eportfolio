<?php

namespace App\Controller;

use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Attribute\Route;

final class PortfolioControleurController extends AbstractController
{
    #[Route('/', name: 'portfolio_home')]
    public function home(): Response
    {
        return $this->render('portfolio_controleur/index.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/loisirs', name: 'portfolio_loisirs')]
    public function index(): Response
    {
        return $this->render('portfolio_controleur/loisirs.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/competences', name: 'portfolio_competences')]
    public function competences(): Response
    {
        return $this->render('portfolio_controleur/competences.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/contact', name: 'portfolio_contact')]
    public function contact(): Response
    {
        return $this->render('portfolio_controleur/contact.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/cv', name: 'portfolio_cv')]
    public function cv(): Response
    {
        return $this->render('portfolio_controleur/cv.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/page-CV', name: 'portfolio_page_cv')]
    public function pagecv(): Response
    {
        return $this->render('portfolio_controleur/page-CV.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
   
}
