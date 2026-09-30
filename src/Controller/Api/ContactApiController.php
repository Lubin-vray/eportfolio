<?php
// src/Controller/Api/ContactApiController.php
namespace App\Controller\Api;


use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpClient\HttpClient;


class ContactApiController extends AbstractController
{
    public function contact(Request $request): JsonResponse
    {
        try {
            $data = json_decode($request->getContent(), true);


            $name = $data['name'] ?? '';
            $email = $data['email'] ?? '';
            $message = $data['message'] ?? '';


            if (!$name || !$email || !$message) {
                return new JsonResponse(['error' => 'Données invalides'], 400);
            }


            $client = HttpClient::create();


            // 1. Sauvegarder dans Supabase
            $supabaseUrl = $_ENV['SUPABASE_URL'];
            $supabaseKey = $_ENV['SUPABASE_ANON_KEY'];
            
            if (!$supabaseUrl || !$supabaseKey) {
                return new JsonResponse(['error' => 'Configuration manquante'], 500);
            }


            $response = $client->request('POST', 
                $supabaseUrl . '/rest/v1/contact_messages',
                [
                    'headers' => [
                        'apikey' => $supabaseKey,
                        'Authorization' => 'Bearer ' . $supabaseKey,
                        'Content-Type' => 'application/json',
                        'Prefer' => 'return=minimal',
                    ],
                    'json' => [
                        'name' => $name,
                        'email' => $email,
                        'message' => $message,
                    ],
                ]
            );


            if ($response->getStatusCode() !== 201) {
                return new JsonResponse(['error' => 'Erreur Supabase'], 500);
            }


            // 2. Envoyer l'email via l'API HTTP Resend
            $resendKey = $_ENV['RESEND_API_KEY'];
            
            if (!$resendKey) {
                return new JsonResponse(['error' => 'Configuration Resend manquante'], 500);
            }


            $emailResponse = $client->request('POST', 'https://api.resend.com/emails', [
                'headers' => [
                    'Authorization' => 'Bearer ' . $resendKey,
                    'Content-Type' => 'application/json',
                ],
                'json' => [
                    'from' => 'onboarding@resend.dev',
                    'to' => ['lubin.vray@etu.univ-st-etienne.fr'],  // ← Ton email Resend
                    'subject' => "Nouveau message de {$name}",
                    'html' => "
                        <h2>Nouveau message depuis le portfolio</h2>
                        <p><strong>Nom :</strong> {$name}</p>
                        <p><strong>Email :</strong> {$email}</p>
                        <p><strong>Message :</strong></p>
                        <p>" . nl2br(htmlspecialchars($message)) . "</p>
                    ",
                ],
            ]);


            if ($emailResponse->getStatusCode() === 200) {
                return new JsonResponse(['success' => true, 'message' => 'Message envoyé !']);
            }


            $errorData = $emailResponse->toArray();
            return new JsonResponse([
                'error' => 'Erreur email',
                'details' => $errorData['message'] ?? 'Erreur inconnue'
            ], 500);


        } catch (\Exception $e) {
            return new JsonResponse([
                'error' => 'Erreur: ' . $e->getMessage()
            ], 500);
        }
    }
}