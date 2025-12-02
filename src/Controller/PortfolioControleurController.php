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
    #[Route('/competences/sae1', name: 'portfolio_sae1')]
    public function sae1(): Response
    {
        return $this->render('portfolio_controleur/sae/sae1.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/competences/sae2', name: 'portfolio_sae2')]
    public function sae2(): Response
    {
        return $this->render('portfolio_controleur/sae/sae2.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/competences/sae3', name: 'portfolio_sae3')]
    public function sae3(): Response
    {
        return $this->render('portfolio_controleur/sae/sae3.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/competences/sae4', name: 'portfolio_sae4')]
    public function sae4(): Response
    {
        return $this->render('portfolio_controleur/sae/sae4.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/competences/sae5', name: 'portfolio_sae5')]
    public function sae5(): Response
    {
        return $this->render('portfolio_controleur/sae/sae5.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
    #[Route('/competences/saeportfolio', name: 'portfolio_saeportfolio')]
    public function saeportfolio(): Response
    {
        return $this->render('portfolio_controleur/sae/saeportfolio.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
   
}
