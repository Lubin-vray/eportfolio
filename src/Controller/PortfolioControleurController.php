<?php

namespace App\Controller;

use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Attribute\Route;

final class PortfolioControleurController extends AbstractController
{
    #[Route('', name: 'app_portfolio_controleur')]
    public function index(): Response
    {
        return $this->render('portfolio_controleur/index.html.twig', [
            'controller_name' => 'PortfolioControleurController',
        ]);
    }
}
