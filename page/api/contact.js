// pages/api/contact.js
import { createClient } from '@supabase/supabase-js';
import { Resend } from 'resend';

export default async function handler(req, res) {
  if (req.method !== 'POST') {
    return res.status(405).json({ error: 'Méthode non autorisée' });
  }

  const { name, email, message } = req.body;

  // 1. Sauvegarder dans Supabase
  const supabase = createClient(
    process.env.SUPABASE_URL,
    process.env.SUPABASE_ANON_KEY
  );

  const { error: dbError } = await supabase
    .from('contact_messages')
    .insert([{ name, email, message }]);

  if (dbError) {
    console.error('Erreur Supabase:', dbError);
    return res.status(500).json({ error: 'Erreur lors de l’enregistrement.' });
  }

  // 2. Envoyer l’email via Resend
  const resend = new Resend(process.env.RESEND_API_KEY);

  const { error: emailError } = await resend.emails.send({
    from: 'Portfolio <onboarding@resend.dev>', // ou ton domaine vérifié
    to: [process.env.YOUR_EMAIL],
    subject: `Nouveau message de ${name}`,
    html: `
      <h2>Nouveau message depuis le portfolio</h2>
      <p><strong>Nom :</strong> ${name}</p>
      <p><strong>Email :</strong> ${email}</p>
      <p><strong>Message :</strong></p>
      <p>${message.replace(/\n/g, '<br>')}</p>
    `,
  });

  if (emailError) {
    console.error('Erreur email:', emailError);
    // On ne bloque pas l’utilisateur, mais on log l’erreur
  }

  return res.status(200).json({ success: true });
}