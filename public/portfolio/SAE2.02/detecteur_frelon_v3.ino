/*
 * ============================================================
 *  SAÉ 2.02 – Détecteur de Frelons Asiatiques  (v5)
 *  Filtre passe-bande 160 Hz + moyenne glissante 100 ms
 * ============================================================
 */

// ── Pins ─────────────────────────────────────────────────────
#define PIN_MIC A1
#define PIN_LED 13
#define PIN_BTN 8

// ── Coefficients filtres ──────────────────────────────────────
// Te = 1 ms, Fe = 1000 Hz
// Passe-bas  Fc = 160 Hz : α = 0.5013
// Passe-haut Fc = 160 Hz : β = 0.4987
const float alpha_PB = 0.5013;
const float beta_PH  = 0.4987;

// ── Suppression DC (Fc ≈ 1 Hz) ───────────────────────────────
const float alpha_DC = 0.001;
float dc_offset      = 512.0;

// ── Enveloppe ─────────────────────────────────────────────────
const float alpha_ENV = 0.05;

// ── Moyenne glissante sur 100 échantillons = 100 ms ──────────
#define TAILLE_FENETRE 300
float fenetre[TAILLE_FENETRE];   // tableau circulaire
int   idx_fenetre  = 0;          // index courant
float somme_gliss  = 0.0;        // somme courante (évite de tout recalculer)
float moy_gliss    = 0.0;        // moyenne sur 100 ms

// ── Seuils sur la MOYENNE glissante ──────────────────────────
const float SEUIL_BAS  = 4.8;   // à ajuster avec le traceur série
const float SEUIL_HAUT = 5.3;   // évite les bruits forts parasites

// ── Variables d'état filtres ──────────────────────────────────
float ek      = 0.0;
float s_PB    = 0.0, s_PB_1  = 0.0;
float s_PB2   = 0.0, s_PB2_1 = 0.0;
float s_PH    = 0.0, s_PH_1  = 0.0;
float env     = 0.0;

bool  ledOn   = false;

// ============================================================
void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  digitalWrite(PIN_LED, LOW);
  Serial.begin(115200);
  Serial.println("brut,centree,passe_bas,passe_bande,enveloppe,moyenne100ms");

  // Initialisation du tableau glissant à 0
  for (int i = 0; i < TAILLE_FENETRE; i++) fenetre[i] = 0.0;

  // Calibration DC initiale sur 100 échantillons
  float somme = 0;
  for (int i = 0; i < 100; i++) {
    somme += analogRead(PIN_MIC);
    delay(1);
  }
  dc_offset = somme / 100.0;
  Serial.print("DC offset initial = ");
  Serial.println(dc_offset);
}

// ============================================================
void loop() {

  // ── 1. Lecture brute ─────────────────────────────────────
  float brut = (float)analogRead(PIN_MIC);

  // ── 2. Mise à jour du DC (~1 Hz) ─────────────────────────
  dc_offset = alpha_DC * brut + (1.0f - alpha_DC) * dc_offset;

  // ── 3. Suppression DC ────────────────────────────────────
  ek = brut - dc_offset;


  // ── 4. Passe-bas 1 (Fc = 160 Hz) ─────────────────────────
  s_PB = 0.758 * ek + (1.0f - 0.758) * s_PB_1;
  // ── 4. Passe-bas 1 (Fc = 160 Hz) ─────────────────────────
  s_PB = alpha_PB * ek + (1.0f - alpha_PB) * s_PB_1;

  // ── 5. Passe-bas 2 en cascade ────────────────────────────
  s_PB2 = alpha_PB * s_PB + (1.0f - alpha_PB) * s_PB2_1;

  // ── 6. Passe-haut (Fc = 160 Hz) → passe-bande centré 160 Hz
  s_PH = beta_PH * (s_PH_1 + s_PB2 - s_PB2_1);

  // ── 7. Enveloppe = valeur absolue lissée ─────────────────
  float abs_PH = s_PH < 0 ? -s_PH : s_PH;
  env = alpha_ENV * abs_PH + (1.0f - alpha_ENV) * env;

  // ── 8. Moyenne glissante sur 100 ms ──────────────────────
  // On retire l'ancienne valeur, on ajoute la nouvelle
  somme_gliss -= fenetre[idx_fenetre];
  fenetre[idx_fenetre] = env;
  somme_gliss += env;
  idx_fenetre = (idx_fenetre + 1) % TAILLE_FENETRE;
  moy_gliss = somme_gliss / TAILLE_FENETRE;

  // ── 9. Détection sur la MOYENNE ──────────────────────────
  if (!ledOn && moy_gliss > SEUIL_BAS && moy_gliss < SEUIL_HAUT) {
    ledOn = true;
    digitalWrite(PIN_LED, HIGH);
    Serial.println(">>> FRELON DETECTE !");
  }

  // ── 10. Reset bouton ─────────────────────────────────────
  if (digitalRead(PIN_BTN) == HIGH) {
    ledOn = false;
    digitalWrite(PIN_LED, LOW);
    Serial.println("--- Reset ---");
    delay(200);
  }

  // ── 11. Mise à jour des états ─────────────────────────────
  s_PB_1  = s_PB;
  s_PB2_1 = s_PB2;
  s_PH_1  = s_PH;

  // ── 12. Traceur Série ─────────────────────────────────────
  // Colonnes : brut | centrée | passe_bas | passe_bande | enveloppe | moyenne100ms
  Serial.print(brut);       Serial.print(",");
  Serial.print(ek);         Serial.print(",");
  Serial.print(s_PB2);      Serial.print(",");
  Serial.print(s_PH);       Serial.print(",");
  Serial.print(env);        Serial.print(",");
  Serial.println(moy_gliss);

  // ── 13. Fe = 1000 Hz ──────────────────────────────────────
  delayMicroseconds(1000);
}