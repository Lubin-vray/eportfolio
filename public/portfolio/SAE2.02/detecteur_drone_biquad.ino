// ================================================================
//  SAE 2.02 – Détecteur acoustique de DRONE
//  Université Jean-Monnet, IUT de Roanne
//
//  PHYSIQUE DES PALES
//  ──────────────────
//  Chaque fois qu'une pale passe devant le corps du drone,
//  elle génère une perturbation de pression. Si le rotor a B
//  pales et tourne à N_rpm tours/minute :
//
//    frot  = N_rpm / 60              (Hz)
//    fBPF  = B × frot                (Hz)  ← fréquence de passage des pales
//
//  Exemple : 2 pales × 6000 tr/min = 2 × 100 Hz = 200 Hz
//
//  Le bruit du drone contient aussi des harmoniques :
//    fk = k × fBPF  (k = 1, 2, 3 …)
//  On surveille la fondamentale (k=1), soit fBPF.
//
//  TABLE DE RÉFÉRENCE (pour 2 pales) :
//    3 000 tr/min → fBPF = 100 Hz
//    6 000 tr/min → fBPF = 200 Hz
//    9 000 tr/min → fBPF = 300 Hz
//   12 000 tr/min → fBPF = 400 Hz
//
//  PARAMÈTRES DU PROJET (à modifier selon votre drone cible)
//  ──────────────────────────────────────────────────────────
//    N_PALES   = 2      nombre de pales par rotor
//    RPM_CIBLE = 6000   vitesse estimée en tr/min
//    => frot   = 100 Hz
//    => fBPF   = 200 Hz   ← fréquence centrale du filtre
//
//  FILTRE : biquad passe-bande (ordre 2, méthode standard Audio EQ)
//  ──────────────────────────────────────────────────────────────────
//  Paramètres :
//    fs  = 2000 Hz   fréquence d'échantillonnage
//    f0  = fBPF      fréquence centrale du filtre
//    Q   = 3         facteur de qualité (bande = f0/Q ≈ 67 Hz)
//
//  Calcul des coefficients :
//    ω0   = 2π × f0 / fs
//    α    = sin(ω0) / (2Q)
//
//    b̃0 = α       b̃1 = 0       b̃2 = −α
//    ã0 = 1+α     ã1 = −2cos(ω0)   ã2 = 1−α
//
//    Normalisation : b_i = b̃_i / ã0   et   a_i = ã_i / ã0
//
//  Équation aux différences (appliquée à chaque échantillon) :
//    y[n] = b0·x[n] + b1·x[n-1] + b2·x[n-2]
//                   − a1·y[n-1] − a2·y[n-2]
//
//  Coefficients calculés pour f0=200 Hz, fs=2000 Hz, Q=3 :
//    b0 =  0.131652    b1 =  0.000000    b2 = -0.131652
//    a1 = -1.414214    a2 =  0.736697
//
//  DÉTECTION RMS
//  ─────────────
//    Fenêtre : NWIN = 200 échantillons = 100 ms à fs=2000 Hz
//    Energie : Em = (1/N) × Σ y²[n]
//    Décision : Em > seuil pendant 3 fenêtres consécutives
//
//  BRANCHEMENTS
//  ────────────
//    Microphone KY-038 :  VCC → 5V  |  GND → GND  |  AO → A0
//    LED de détection  :  broche 13 (LED intégrée Arduino)
//    Bouton reset      :  broche 2  → GND (INPUT_PULLUP)
// ================================================================

#include <math.h>   // pour M_PI, sin(), cos() utilisés dans calcul_coefficients()

// ── Broches ────────────────────────────────────────────────────
const int MIC_PIN = A1;   // sortie analogique du microphone
const int LED_PIN = 13;   // LED de détection (intégrée)
const int BTN_PIN = 8;    // bouton reset (actif LOW, INPUT_PULLUP)

// ================================================================
//  CONFIGURATION PHYSIQUE DU DRONE CIBLE
//  Modifier ces trois valeurs selon le drone que vous surveillez
// ================================================================
const int   N_PALES   = 2;       // nombre de pales par rotor
const float RPM_CIBLE = 6000.0;  // vitesse de rotation estimée (tr/min)
const float Q_FILTRE  = 3.0;     // facteur de qualité du filtre biquad
//   Q élevé (5–10) → bande étroite, très sélectif
//   Q faible (1–2) → bande large, tolère des variations de RPM

// ================================================================
//  FRÉQUENCE D'ÉCHANTILLONNAGE
// ================================================================
const float FS    = 2000.0;  // Hz  – doit être > 2 × fBPF (Shannon)
const int   TS_US = 500;     // µs  – 1 / FS en microsecondes

// ================================================================
//  CALCUL AUTOMATIQUE DE fBPF À PARTIR DE LA PHYSIQUE DES PALES
//
//    frot = RPM_CIBLE / 60
//    fBPF = N_PALES × frot
//
//  Ces lignes s'exécutent une seule fois, à la compilation.
// ================================================================
const float F_ROT = RPM_CIBLE / 60.0;            // fréquence de rotation (Hz)
const float F_BPF = (float)N_PALES * F_ROT;      // fréquence de passage des pales (Hz)
// F_BPF devient la fréquence centrale f0 du filtre passe-bande

// ================================================================
//  COEFFICIENTS DU FILTRE BIQUAD PASSE-BANDE
//
//  Calculés pour : f0 = F_BPF = 200 Hz, fs = 2000 Hz, Q = 3
//
//    ω0   = 2π × 200 / 2000 = 0.6283 rad
//    α    = sin(0.6283) / (2×3) = 0.5878 / 6 = 0.09797
//
//    ã0 = 1 + 0.09797 = 1.09797
//    ã1 = −2 × cos(0.6283) = −2 × 0.8090 = −1.6180
//    ã2 = 1 − 0.09797 = 0.90203
//
//  Après normalisation par ã0 :
//    b0 =  0.09797 / 1.09797 =  0.08923
//    b1 =  0
//    b2 = −0.09797 / 1.09797 = −0.08923
//    a1 = −1.6180  / 1.09797 = −1.47364
//    a2 =  0.90203 / 1.09797 =  0.82155
//
//  NOTE : si vous changez N_PALES, RPM_CIBLE, Q_FILTRE ou FS,
//         recalculez ces coefficients (voir fonction calcul_coefficients
//         en bas de fichier) et copiez les nouvelles valeurs ici.
// ================================================================
float b0 =  0.08923f;
float b1 =  0.00000f;
float b2 = -0.08923f;
float a1 = -1.47364f;
float a2 =  0.82155f;

// ── Mémoires du filtre biquad (deux retards en entrée et sortie) ─
float x1 = 0.0f, x2 = 0.0f;   // x[n-1] et x[n-2]
float y1 = 0.0f, y2 = 0.0f;   // y[n-1] et y[n-2]

// ================================================================
//  PARAMÈTRES DE DÉTECTION RMS
// ================================================================
const int   NWIN      = 200;    // échantillons / fenêtre = 100 ms à 2000 Hz
const int   N_VALID   = 3;      // fenêtres consécutives requises pour valider
float       threshold = 300.0f; // seuil d'énergie  ← à calibrer (voir setup)

// ── Accumulateurs ───────────────────────────────────────────────
float energySum   = 0.0f;
int   sampleCount = 0;
int   countAbove  = 0;   // compteur de fenêtres consécutives > seuil
bool  detected    = false;

// ================================================================
//  SETUP
// ================================================================
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  while (!Serial) {}   // attendre l'ouverture du port sur Uno/Leonardo

  // ── Affichage de la configuration au démarrage ─────────────
  Serial.println(F("================================================"));
  Serial.println(F("  Detecteur acoustique de DRONE – biquad RMS"));
  Serial.println(F("================================================"));

  Serial.println(F("--- Physique des pales ---"));
  Serial.print(F("  Nombre de pales   B  = "));  Serial.println(N_PALES);
  Serial.print(F("  Vitesse estimee      = "));  Serial.print(RPM_CIBLE, 0); Serial.println(F(" tr/min"));
  Serial.print(F("  frot = RPM/60        = "));  Serial.print(F_ROT, 2);    Serial.println(F(" Hz"));
  Serial.print(F("  fBPF = B x frot      = "));  Serial.print(F_BPF, 2);    Serial.println(F(" Hz  <-- frequence centrale du filtre"));

  Serial.println(F("--- Filtre biquad passe-bande ---"));
  Serial.print(F("  f0 = fBPF = "));  Serial.print(F_BPF, 1);  Serial.println(F(" Hz"));
  Serial.print(F("  Q  = "));         Serial.println(Q_FILTRE, 1);
  Serial.print(F("  fs = "));         Serial.print(FS, 0);      Serial.println(F(" Hz"));
  Serial.print(F("  Bande ≈ f0/Q = "));
  Serial.print(F_BPF / Q_FILTRE, 1); Serial.println(F(" Hz"));
  Serial.println(F("  Coefficients normalises :"));
  Serial.print(F("    b0 = "));  Serial.println(b0, 5);
  Serial.print(F("    b1 = "));  Serial.println(b1, 5);
  Serial.print(F("    b2 = "));  Serial.println(b2, 5);
  Serial.print(F("    a1 = "));  Serial.println(a1, 5);
  Serial.print(F("    a2 = "));  Serial.println(a2, 5);

  Serial.println(F("--- Detection RMS ---"));
  Serial.print(F("  Fenetre = "));  Serial.print(NWIN);
  Serial.print(F(" pts = "));       Serial.print((float)NWIN / FS * 1000.0f, 0);
  Serial.println(F(" ms"));
  Serial.print(F("  Validation : ")); Serial.print(N_VALID);
  Serial.println(F(" fenetres consecutives"));
  Serial.print(F("  Seuil initial = ")); Serial.println(threshold);
  Serial.println(F("  >> Ajuster 'threshold' apres calibration"));
  Serial.println(F("================================================"));
  Serial.println(F("Format serie : Em | Etat | [Decision]"));

  // ── Recalcul des coefficients si les constantes ont changé ───
  // Décommentez cette ligne pour recalculer automatiquement :
  // calcul_coefficients(F_BPF, FS, Q_FILTRE);
}

// ================================================================
//  BOUCLE PRINCIPALE
// ================================================================
void loop() {
  // ── Horodatage : assure Fe = 2000 Hz ──────────────────────────
  unsigned long t0 = micros();

  // ── 1. Acquisition et centrage ────────────────────────────────
  //  analogRead → entier 0–1023 (CAN 10 bits, 0–5 V)
  //  On soustrait 512 pour centrer autour de 0 :
  //    x[n] = u[n] − 512
  //  Sans ce centrage, l'énergie RMS serait dominée par la
  //  composante continue (offset DC) et ne refléterait pas
  //  le contenu acoustique utile.
  float x0 = (float)analogRead(MIC_PIN) - 512.0f;

  // ── 2. Filtre biquad passe-bande ──────────────────────────────
  //
  //  Équation aux différences :
  //    y[n] = b0·x[n] + b1·x[n-1] + b2·x[n-2]
  //                   − a1·y[n-1] − a2·y[n-2]
  //
  //  b0 et b2 sont opposés (b1 = 0) : c'est la signature
  //  du filtre passe-bande symétrique.
  //  Les termes en a1, a2 forment la partie récursive (IIR)
  //  qui crée la sélectivité en fréquence.
  //
  float y0 = b0 * x0
           + b1 * x1
           + b2 * x2
           - a1 * y1
           - a2 * y2;

  // Mise à jour des registres à décalage
  x2 = x1;  x1 = x0;   // décalage de l'entrée
  y2 = y1;  y1 = y0;   // décalage de la sortie

  // ── 3. Accumulation d'énergie ─────────────────────────────────
  //  On accumule y²[n]. La mise au carré rend toutes les
  //  contributions positives (le signal oscille +/−).
  energySum += y0 * y0;
  sampleCount++;

  // ── 4. Traitement par fenêtres de NWIN = 100 ms ───────────────
  if (sampleCount >= NWIN) {

    // Énergie moyenne de la fenêtre : Em = (1/N) × Σ y²
    float Em = energySum / (float)NWIN;

    // Affichage pour calibration et débogage
    Serial.print(F("Em="));
    Serial.print(Em, 1);
    Serial.print(F(" | Seuil="));
    Serial.print(threshold, 1);
    Serial.print(F(" | fBPF="));
    Serial.print(F_BPF, 0);
    Serial.print(F("Hz | "));

    // ── Décision binaire sur la fenêtre ──────────────────────
    //    Dm = 1 si Em > seuil, 0 sinon
    if (Em > threshold) {
      countAbove++;
      Serial.print(F("[SIGNAL]"));
    } else {
      countAbove = 0;   // on repart à zéro dès qu'une fenêtre passe sous le seuil
      Serial.print(F("[bruit] "));
    }

    // ── Validation sur N_VALID = 3 fenêtres consécutives ─────
    //  Principe : un bruit bref (clap, choc) peut dépasser le
    //  seuil une seule fois. Un drone en vol produit un signal
    //  continu → au moins 3 × 100 ms = 300 ms au-dessus du seuil.
    if (countAbove >= N_VALID && !detected) {
      detected = true;
      digitalWrite(LED_PIN, HIGH);
      Serial.print(F(" >>> DRONE DETECTE (fBPF="));
      Serial.print(F_BPF, 0);
      Serial.print(F(" Hz, "));
      Serial.print(N_PALES);
      Serial.print(F(" pales x "));
      Serial.print(F_ROT, 0);
      Serial.println(F(" Hz rot) <<<"));
    } else {
      Serial.println();
    }

    // Remise à zéro de l'accumulateur pour la prochaine fenêtre
    energySum   = 0.0f;
    sampleCount = 0;
  }

  // ── 5. Gestion du bouton reset ────────────────────────────────
  //  INPUT_PULLUP → broche à HIGH au repos, LOW quand appuyée.
  //  On réinitialise seulement si une détection est en cours.
  if (digitalRead(BTN_PIN) == HIGH && detected) {
    detected   = false;
    countAbove = 0;
    digitalWrite(LED_PIN, LOW);
    Serial.println(F(">>> Reset – en attente de signal <<<"));
    delay(250);   // anti-rebond
  }

  // ── 6. Synchronisation temporelle ────────────────────────────
  //  Boucle d'attente active : on attend que 500 µs (= 1/2000 s)
  //  se soient écoulées depuis t0. Plus précis que delay(1).
  while (micros() - t0 < (unsigned long)TS_US) { /* busy-wait */ }
}

// ================================================================
//  FONCTION UTILITAIRE : recalcul dynamique des coefficients
//
//  Appelez calcul_coefficients(f0, fs, Q) dans setup() si vous
//  modifiez N_PALES, RPM_CIBLE ou Q_FILTRE, pour mettre à jour
//  b0, b1, b2, a1, a2 automatiquement sans ressaisir les valeurs.
//
//  Formules (filtre biquad passe-bande, Audio EQ Cookbook) :
//    ω0 = 2π × f0 / fs
//    α  = sin(ω0) / (2Q)
//
//    b̃0 =  α      b̃1 = 0     b̃2 = −α
//    ã0 = 1+α     ã1 = −2cos(ω0)   ã2 = 1−α
//
//    b_i = b̃_i / ã0    a_i = ã_i / ã0
// ================================================================
void calcul_coefficients(float f0, float fs, float Q) {
  float w0    = 2.0f * (float)M_PI * f0 / fs;
  float alpha = sinf(w0) / (2.0f * Q);
  float a0    = 1.0f + alpha;

  b0 =  alpha / a0;
  b1 =  0.0f;
  b2 = -alpha / a0;
  a1 = (-2.0f * cosf(w0)) / a0;
  a2 = (1.0f - alpha) / a0;

  // Réinitialisation des mémoires du filtre après changement de coefficients
  x1 = x2 = 0.0f;
  y1 = y2 = 0.0f;

  Serial.println(F("--- Coefficients recalcules ---"));
  Serial.print(F("  f0="));  Serial.print(f0, 1);
  Serial.print(F(" fs="));   Serial.print(fs, 0);
  Serial.print(F(" Q="));    Serial.println(Q, 2);
  Serial.print(F("  b0=")); Serial.print(b0, 5);
  Serial.print(F("  b2=")); Serial.println(b2, 5);
  Serial.print(F("  a1=")); Serial.print(a1, 5);
  Serial.print(F("  a2=")); Serial.println(a2, 5);
}

// ================================================================
//  PROCÉDURE DE CALIBRATION DU SEUIL
// ================================================================
//
//  1. Téléverser le programme. Ouvrir le Moniteur Série (115200 bd).
//  2. Laisser tourner 30 s en silence (pas de drone, pas de source).
//     → Observer les valeurs "Em=" affichées.
//     → Noter Em_max (valeur maximale du bruit de fond).
//  3. Fixer threshold = Em_max × 3
//     Règle générale : threshold = µ_bruit + k × σ_bruit  (k = 3 à 5)
//  4. Re-téléverser avec la nouvelle valeur de threshold.
//
//  TEST DE FONCTIONNEMENT :
//  → Générer un son à f0 = F_BPF Hz avec l'appli "Tone Generator".
//    Approcher du microphone → LED doit s'allumer après ~300 ms.
//  → Tester avec f0/2 et 2×f0 : LED NE doit PAS s'allumer.
//  → Appuyer sur le bouton pour éteindre et recommencer.
//
//  RÉPONSES AUX QUESTIONS DU DOCUMENT DE TRAVAIL :
//
//  Q : Calcul frot pour 6000 tr/min ?
//  R : frot = 6000 / 60 = 100 Hz
//
//  Q : fBPF pour 2 pales ?
//  R : fBPF = 2 × 100 = 200 Hz   ← c'est f0 du filtre
//
//  Q : fBPF pour 3 pales ?
//  R : fBPF = 3 × 100 = 300 Hz   (changer N_PALES = 3)
//
//  Q : Pourquoi des harmoniques 2fBPF, 3fBPF ?
//  R : La forme d'onde de pression n'est pas sinusoïdale.
//      Sa décomposition de Fourier contient des multiples de fBPF.
//
//  Q : Facteur de qualité pour f0=240 Hz, Δf=80 Hz ?
//  R : Q = f0 / Δf = 240 / 80 = 3
//
//  Q : Ts pour fs = 2000 Hz ?
//  R : Ts = 1 / 2000 = 0.5 ms = 500 µs
//
//  Q : Durée d'une fenêtre (N=200, fs=2000) ?
//  R : 200 / 2000 = 0.1 s = 100 ms
//
//  Q : Décisions par seconde ?
//  R : 2000 / 200 = 10 décisions/s
//
//  Q : 1ère détection validée dans la figure ?
//  R : À la fenêtre 9 (3 fenêtres consécutives au-dessus du seuil).
//
//  Q : Seuil trop bas ?
//  R : Fausses alertes : le bruit ambiant déclenche la LED.
//
//  Q : Seuil trop haut ?
//  R : Manques de détection : un drone réel n'est pas détecté.
// ================================================================
