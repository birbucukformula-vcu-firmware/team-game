#include "raylib.h"
#include <math.h>

// --- ENUMLAR VE YAPILAR ---
typedef enum { MENU, GARAJ, OYUN, BITIS } OyunDurumu;
typedef enum { ASFALT, TOPRAK, GECE } HaritaTipi;

typedef struct {
    Vector2 pozisyon;
    Vector2 boyut;
    Color renk;
    int modelTipi; // 0: Kare, 1: Dikdörtgen, 2: Üçgen
} Araba;

typedef struct {
    Vector2 pozisyon;
} Nesne;

typedef struct {
    Color yolRengi;
    Color cizgiRengi;
    Color arkaPlan;
} Harita;

// --- KÜRESEL DEĞİŞKENLER VE SAHİPLİK ---
bool renkSahiplik[12] = { true, false, false, false, false, false, false, false, false, false, false, false };
bool sekilSahiplik[3] = { true, false, false };

// --- FONKSİYONLAR ---

// Tekerlekleri çizen fonksiyon (Gece modunda gri olur)
void DrawCarWheels(Vector2 pos, Vector2 size, HaritaTipi harita) {
    Color wheelColor = (harita == GECE) ? LIGHTGRAY : BLACK;
    float wheelW = size.x * 0.22f;
    float wheelH = size.y * 0.28f;
    // Tekerleklerin konumları gövdeye göre ayarlı
    DrawEllipse(pos.x, pos.y + size.y * 0.2f, wheelW, wheelH, wheelColor);
    DrawEllipse(pos.x + size.x, pos.y + size.y * 0.2f, wheelW, wheelH, wheelColor);
    DrawEllipse(pos.x, pos.y + size.y * 0.8f, wheelW, wheelH, wheelColor);
    DrawEllipse(pos.x + size.x, pos.y + size.y * 0.8f, wheelW, wheelH, wheelColor);
}

// Güvenli doğma: Nesnelerin üst üste binmesini önler ve yola sabitler
void GuvenliSpawn(Nesne *hedef, Nesne digerleri[], int adet, int yollar[], int yBaslangic) {
    int secilenYol = GetRandomValue(0, 2);
    hedef->pozisyon.x = (float)yollar[secilenYol];
    hedef->pozisyon.y = (float)yBaslangic;

    for (int i = 0; i < adet; i++) {
        if (&digerleri[i] == hedef) continue;
        if (fabsf(hedef->pozisyon.y - digerleri[i].pozisyon.y) < 220) {
            hedef->pozisyon.y -= 250;
        }
    }
}

int main(void) {
    // Pencere Ayarları
    const int ekranGenislik = 800;
    const int ekranYukseklik = 600;
    InitWindow(ekranGenislik, ekranYukseklik, "Formula 1 - Tam Surum");

    // Oyun Değişkenleri
    OyunDurumu durum = MENU;
    HaritaTipi seciliHarita = ASFALT;
    int garajSayfasi = 0; // 0: Renkler, 1: Şekiller
    int toplamPara = 200; // Başlangıç parası
    int mevcutPara = 0;
    float mevcutSkor = 0, enYuksekSkor = 0;
    float temelHiz = 7.5f, akisHizi = temelHiz;
    float yolCizgiOffset = 0;
    int yollar[3] = { 265, 400, 535 };

    // Araba ve Nesne Tanımları
    Araba araba = { {365, 450}, {70, 70}, WHITE, 0 };
    Nesne engeller[3];
    Nesne para = { {0, -1500} };
    Nesne tumNesneler[4];

    // Harita Renk Tanımları
    Harita haritalar[3] = {
        { GRAY, WHITE, DARKGRAY },     // ASFALT
        { BROWN, DARKBROWN, GREEN },   // TOPRAK
        { BLACK, SKYBLUE, DARKBLUE }   // GECE
    };

    // Market Ayarları
    Color marketRenkleri[12] = { 
        WHITE, RED, BLUE, YELLOW, GREEN, PURPLE, 
        ORANGE, PINK, LIME, GOLD, SKYBLUE, MAROON 
    };
    int renkFiyat = 30;
    int sekilFiyat = 75;

    // İlk Nesne Yerleşimi
    for (int i = 0; i < 3; i++) {
        engeller[i].pozisyon.x = (float)yollar[i];
        engeller[i].pozisyon.y = -300 - (i * 300);
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- GÜNCELLEME ---
        if (durum == OYUN) {
            mevcutSkor += 0.15f;
            akisHizi = temelHiz + ((int)(mevcutSkor / 100) * 1.3f);
            yolCizgiOffset += akisHizi;
            if (yolCizgiOffset >= 100) yolCizgiOffset = 0;

            // Kontroller
            if (IsKeyDown(KEY_LEFT) && araba.pozisyon.x > 210) araba.pozisyon.x -= (akisHizi * 0.8f + 2);
            if (IsKeyDown(KEY_RIGHT) && araba.pozisyon.x < 590 - araba.boyut.x) araba.pozisyon.x += (akisHizi * 0.8f + 2);

            Rectangle arabaRect = { araba.pozisyon.x, araba.pozisyon.y, araba.boyut.x, araba.boyut.y };

            // Engeller
            for (int i = 0; i < 3; i++) {
                engeller[i].pozisyon.y += akisHizi;
                tumNesneler[i] = engeller[i];
                if (engeller[i].pozisyon.y > ekranYukseklik) GuvenliSpawn(&engeller[i], tumNesneler, 4, yollar, -200);
                
                if (CheckCollisionRecs(arabaRect, (Rectangle){engeller[i].pozisyon.x - 40, engeller[i].pozisyon.y, 80, 80})) {
                    toplamPara += mevcutPara;
                    if (mevcutSkor > enYuksekSkor) enYuksekSkor = mevcutSkor;
                    durum = BITIS;
                }
            }

            // Para
            para.pozisyon.y += akisHizi;
            tumNesneler[3] = para;
            if (para.pozisyon.y > ekranYukseklik) GuvenliSpawn(&para, tumNesneler, 4, yollar, -500);
            if (CheckCollisionCircleRec(para.pozisyon, 25, arabaRect)) {
                mevcutPara += 10;
                para.pozisyon.y = -600;
            }
        } 
        else if (durum == GARAJ) {
            if (IsKeyPressed(KEY_RIGHT)) garajSayfasi = 1;
            if (IsKeyPressed(KEY_LEFT)) garajSayfasi = 0;

            if (garajSayfasi == 0) {
                // Renk Seçimi Tuşları (1-9 ve Q, W, E)
                int tuslar[] = { KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_Q, KEY_W, KEY_E };
                for (int i = 0; i < 12; i++) {
                    if (IsKeyPressed(tuslar[i])) {
                        if (renkSahiplik[i]) {
                            araba.renk = marketRenkleri[i];
                        } else if (toplamPara >= renkFiyat) {
                            toplamPara -= renkFiyat;
                            renkSahiplik[i] = true;
                            araba.renk = marketRenkleri[i];
                        }
                    }
                }
            } else {
                // Şekil Seçimi Tuşları
                if (IsKeyPressed(KEY_Q)) {
                    if (sekilSahiplik[0]) { araba.boyut = (Vector2){70, 70}; araba.modelTipi = 0; }
                    else if (toplamPara >= sekilFiyat) { toplamPara -= sekilFiyat; sekilSahiplik[0] = true; araba.boyut = (Vector2){70, 70}; araba.modelTipi = 0; }
                }
                if (IsKeyPressed(KEY_W)) {
                    if (sekilSahiplik[1]) { araba.boyut = (Vector2){50, 100}; araba.modelTipi = 1; }
                    else if (toplamPara >= sekilFiyat) { toplamPara -= sekilFiyat; sekilSahiplik[1] = true; araba.boyut = (Vector2){50, 100}; araba.modelTipi = 1; }
                }
                if (IsKeyPressed(KEY_E)) {
                    if (sekilSahiplik[2]) { araba.boyut = (Vector2){70, 90}; araba.modelTipi = 2; }
                    else if (toplamPara >= sekilFiyat) { toplamPara -= sekilFiyat; sekilSahiplik[2] = true; araba.boyut = (Vector2){70, 90}; araba.modelTipi = 2; }
                }
            }
            if (IsKeyPressed(KEY_M)) durum = MENU;
        } 
        else { // MENU veya BITIS
            if (IsKeyPressed(KEY_ENTER)) {
                mevcutPara = 0; mevcutSkor = 0; akisHizi = temelHiz;
                araba.pozisyon.x = 365;
                for (int i = 0; i < 3; i++) { engeller[i].pozisyon.x = (float)yollar[i]; engeller[i].pozisyon.y = -300 - (i * 300); }
                durum = OYUN;
            }
            if (IsKeyPressed(KEY_G)) durum = GARAJ;
            if (IsKeyPressed(KEY_LEFT)) seciliHarita = (seciliHarita == 0) ? 2 : seciliHarita - 1;
            if (IsKeyPressed(KEY_RIGHT)) seciliHarita = (seciliHarita == 2) ? 0 : seciliHarita + 1;
        }

        // --- ÇİZİM ---
        BeginDrawing();
        ClearBackground(haritalar[seciliHarita].arkaPlan);

        if (durum == OYUN) {
            DrawRectangle(200, 0, 400, 600, haritalar[seciliHarita].yolRengi);
            for (int i = -1; i < 7; i++) {
                DrawRectangle(330, i * 100 + (int)yolCizgiOffset, 6, 50, haritalar[seciliHarita].cizgiRengi);
                DrawRectangle(465, i * 100 + (int)yolCizgiOffset, 6, 50, haritalar[seciliHarita].cizgiRengi);
            }
            
            // Para
            DrawCircleV(para.pozisyon, 25, GOLD);
            DrawText("$", (int)para.pozisyon.x - 7, (int)para.pozisyon.y - 12, 25, (seciliHarita == GECE ? DARKGRAY : BLACK));

            // Engeller
            for (int i = 0; i < 3; i++) {
                DrawRectangle((int)engeller[i].pozisyon.x - 40, (int)engeller[i].pozisyon.y, 80, 80, MAROON);
                DrawRectangleLines((int)engeller[i].pozisyon.x - 40, (int)engeller[i].pozisyon.y, 80, 80, BLACK);
            }

            // Araba ve Tekerlekler
            DrawCarWheels(araba.pozisyon, araba.boyut, seciliHarita);
            if (araba.modelTipi == 2) {
                DrawTriangle((Vector2){araba.pozisyon.x + araba.boyut.x/2, araba.pozisyon.y}, 
                             (Vector2){araba.pozisyon.x, araba.pozisyon.y + araba.boyut.y}, 
                             (Vector2){araba.pozisyon.x + araba.boyut.x, araba.pozisyon.y + araba.boyut.y}, araba.renk);
            } else {
                DrawRectangleV(araba.pozisyon, araba.boyut, araba.renk);
            }

            DrawText(TextFormat("SKOR: %.0f", mevcutSkor), 20, 20, 25, WHITE);
            DrawText(TextFormat("PARA: %i $", mevcutPara), 20, 55, 25, GOLD);
        } 
        else if (durum == GARAJ) {
            DrawRectangle(30, 30, 740, 540, ColorAlpha(BLACK, 0.9f));
            DrawText(TextFormat("KASA: %i $", toplamPara), 320, 70, 25, GREEN);
            
            if (garajSayfasi == 0) {
                DrawText("RENK MARKETI (30$)", 280, 110, 25, GOLD);
                const char* tusIsimleri[] = {"1","2","3","4","5","6","7","8","9","Q","W","E"};
                for (int i = 0; i < 12; i++) {
                    DrawRectangle(80 + (i % 6) * 115, 180 + (i / 6) * 140, 85, 85, marketRenkleri[i]);
                    DrawText(tusIsimleri[i], 115 + (i % 6) * 115, 275 + (i / 6) * 140, 20, WHITE);
                    DrawText(renkSahiplik[i] ? "ALINDI" : "30$", 95 + (i % 6) * 115, 155 + (i / 6) * 140, 18, renkSahiplik[i] ? LIME : GOLD);
                }
                DrawText("SEKILLER ICIN [SAG OK]", 300, 500, 20, LIGHTGRAY);
            } else {
                DrawText("SEKIL MARKETI (75$)", 280, 110, 25, GOLD);
                // Kare
                DrawText(sekilSahiplik[0] ? "SECILDI" : "75$", 100, 220, 20, LIME);
                DrawRectangle(100, 250, 80, 80, araba.renk); DrawText("[Q] KARE", 100, 345, 20, WHITE);
                // Dikdörtgen
                DrawText(sekilSahiplik[1] ? "ALINDI" : "75$", 350, 220, 20, LIME);
                DrawRectangle(350, 240, 50, 100, araba.renk); DrawText("[W] DIKDORTGEN", 310, 355, 20, WHITE);
                // Üçgen
                DrawText(sekilSahiplik[2] ? "ALINDI" : "75$", 575, 220, 20, LIME);
                DrawTriangle((Vector2){600, 240}, (Vector2){560, 340}, (Vector2){640, 340}, araba.renk); DrawText("[E] UCGEN", 570, 355, 20, WHITE);
                
                DrawText("RENKLER ICIN [SOL OK]", 300, 500, 20, LIGHTGRAY);
            }
            DrawText("MENU: [M]", 350, 540, 20, RAYWHITE);
        } 
        else {
            DrawRectangle(0,0,800,600, ColorAlpha(BLACK, 0.4f));
            DrawText(durum == MENU ? "GEOMETRIK FORMULA" : "HURDAYA DONDUN!", 200, 180, 40, GOLD);
            DrawText(TextFormat("HARITA: %s", (seciliHarita==0?"ASFALT":(seciliHarita==1?"TOPRAK":"GECE"))), 280, 280, 25, haritalar[seciliHarita].cizgiRengi);
            DrawText("DEGISTIR: [<- ->]  BASLA: [ENTER]  GARAJ: [G]", 160, 450, 20, RAYWHITE);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}