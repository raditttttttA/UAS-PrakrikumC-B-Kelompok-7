#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/fitur.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0777)
#endif

// Helper pencarian substring tanpa peduli huruf besar/kecil (aman lintas OS)
static int mengandungKataCaseInsensitive(const char *teks, const char *kataKunci) {
    if (!teks || !kataKunci || *kataKunci == '\0') return 0;

    size_t lenTeks = strlen(teks);
    size_t lenKunci = strlen(kataKunci);
    if (lenKunci > lenTeks) return 0;

    for (size_t i = 0; i <= lenTeks - lenKunci; i++) {
        size_t j = 0;
        while (j < lenKunci && tolower((unsigned char)teks[i + j]) == tolower((unsigned char)kataKunci[j])) {
            j++;
        }
        if (j == lenKunci) {
            return 1;
        }
    }
    return 0;
}

// Fungsi internal untuk memastikan folder 'data/' ada secara otomatis
static void pastikanFolderAda() {
    MKDIR("data");
}

// Fungsi manipulasi struct CatatanKeuangan menggunakan pointer (->)
void isiDataTransaksi(CatatanKeuangan *t, int id, const char *tanggal, const char *kategori, const char *tipe, double nominal) {
    t->id = id;

    // Salin string dengan batas buffer aman
    strncpy(t->tanggal, tanggal, sizeof(t->tanggal) - 1);
    t->tanggal[sizeof(t->tanggal) - 1] = '\0';

    strncpy(t->kategori, kategori, sizeof(t->kategori) - 1);
    t->kategori[sizeof(t->kategori) - 1] = '\0';

    if (strcasecmp(tipe, "pemasukan") == 0 || strstr(tipe, "masuk") != NULL) {
        strcpy(t->tipe, "Pemasukan");
    } else {
        strcpy(t->tipe, "Pengeluaran");
    }

    t->nominal = nominal;
}

void tambahTransaksi(const char *filename) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "ab"); // Mode append binary
    if (!file) {
        printf("[ERROR] Gagal membuka file database!\n");
        return;
    }

    transaksi.id = dapatkanIdBerikutnya(filename);

    printf("\n=== TAMBAH TRANSAKSI BARU (Auto ID: #%d) ===\n", transaksi.id);

    printf("Masukkan Tanggal (YYYY-MM-DD): ");
    if (scanf("%14s", transaksi.tanggal) != 1 || strlen(transaksi.tanggal) < 8) {
        printf("[ERROR] Format tanggal tidak valid!\n");
        while (getchar() != '\n');
        fclose(file);
        return;
    }
    while (getchar() != '\n');

    int pilihanTipe;
    printf("Pilih Tipe Transaksi:\n  1. Pemasukan\n  2. Pengeluaran\nPilihan (1/2): ");
    if (scanf("%d", &pilihanTipe) != 1 || (pilihanTipe != 1 && pilihanTipe != 2)) {
        printf("[ERROR] Pilihan harus 1 atau 2!\n");
        while (getchar() != '\n');
        fclose(file);
        return;
    }
    while (getchar() != '\n');

    if (pilihanTipe == 1) {
        strcpy(transaksi.tipe, "Pemasukan");
    } else {
        strcpy(transaksi.tipe, "Pengeluaran");
    }

    printf("Masukkan Kategori (cth: Makanan/Gaji): ");
    if (scanf("%29s", transaksi.kategori) != 1) {
        printf("[ERROR] Input kategori tidak valid!\n");
        while (getchar() != '\n');
        fclose(file);
        return;
    }
    while (getchar() != '\n');

    printf("Masukkan Nominal (Rp): ");
    if (scanf("%lf", &transaksi.nominal) != 1 || transaksi.nominal <= 0) {
        printf("[ERROR] Nominal harus berupa angka positif!\n");
        while (getchar() != '\n');
        fclose(file);
        return;
    }
    while (getchar() != '\n');

    if (fwrite(&transaksi, sizeof(CatatanKeuangan), 1, file) != 1) {
        printf("[ERROR] Gagal menulis data ke file!\n");
        fclose(file);
        return;
    }

    fclose(file);
    printf("[SUKSES] Berhasil menyimpan transaksi!\n");
}

void tampilkanRiwayat(const char *filename) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "rb"); 
    if (!file) {
        printf("\n [INFO]Belum ada data transaksi atau file belum dibuat.\n");
        return;
    }

    printf("\n=====================================================================\n");
    printf("| %-4s | %-12s | %-15s | %-13s | %-14s |\n", "ID", "Tanggal", "Kategori", "Tipe", "Nominal");
    printf("=====================================================================\n");
    
    int count = 0;
    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, file) == 1) {
        printf("| %-4d | %-12s | %-15s | %-13s | Rp%12.2f |\n", 
               transaksi.id, transaksi.tanggal, transaksi.kategori, transaksi.tipe, transaksi.nominal);
        count++;
    }
    printf("=====================================================================\n");
    printf("Total data ditampilkan: %d transaksi.\n", count);
    fclose(file);
}

void cariTransaksi(const char *filename, const char *keyword) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "rb");
    int ditemukan = 0;

    if (!file) {
        printf("[INFO] Database tidak ditemukan.\n");
        return;
    }

    printf("\n=== HASIL PENCARIAN DENGAN KATA KUNCI: '%s' ===\n", keyword);
    printf("=====================================================================\n");
    printf("| %-4s | %-12s | %-15s | %-13s | %-14s |\n", "ID", "Tanggal", "Kategori", "Tipe", "Nominal");
    printf("=====================================================================\n");

    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, file) == 1) {
        // Pencarian aman (case-insensitive atau pencocokan string kategori/tipe)
        if (mengandungKataCaseInsensitive(transaksi.kategori, keyword) ||
            mengandungKataCaseInsensitive(transaksi.tipe, keyword)) {
            printf("| %-4d | %-12s | %-15s | %-13s | Rp%12.2f |\n", 
                   transaksi.id, transaksi.tanggal, transaksi.kategori, transaksi.tipe, transaksi.nominal);
            ditemukan++;
        }
    }
    printf("=====================================================================\n");
    if (!ditemukan) {
        printf("[INFO] Tidak ada data yang cocok dengan '%s'.\n", keyword);
    } else {
        printf("Ditemukan %d data yang cocok.\n", ditemukan);
    }
    fclose(file);
}

void eksporKeTeks(const char *filenameBinary, const char *filenameTeks) {
    pastikanFolderAda(); // Pastikan folder data ada
    CatatanKeuangan transaksi;
    FILE *binFile = fopen(filenameBinary, "rb");
    if (!binFile) {
        printf("[ERROR] Gagal membuka file database binary untuk diekspor.\n");
        return;
    }

    FILE *txtFile = fopen(filenameTeks, "w");
    if (!txtFile) {
        printf("[ERROR] Gagal membuat file laporan teks (%s).\n", filenameTeks);
        fclose(binFile);
        return;
    }

    fprintf(txtFile, "=======================================\n");
    fprintf(txtFile, "      LAPORAN KEUANGAN BULANAN          \n");
    fprintf(txtFile, "=======================================\n");
    fprintf(txtFile, "%-4s | %-12s | %-15s | %-12s | %-12s\n", "ID", "Tanggal", "Kategori", "Tipe", "Nominal");
    fprintf(txtFile, "---------------------------------------------------------\n");
    
    double totalMasuk = 0, totalKeluar = 0;
    int count = 0;

    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, binFile) == 1) {
        fprintf(txtFile, "%-4d | %-12s | %-15s | %-12s | Rp%.2f\n", 
                transaksi.id, transaksi.tanggal, transaksi.kategori, transaksi.tipe, transaksi.nominal);
        
        if (strcasecmp(transaksi.tipe, "Pemasukan") == 0) {
            totalMasuk += transaksi.nominal;
        } else {
            totalKeluar += transaksi.nominal;
        }
        count++;
    }

    fprintf(txtFile, "---------------------------------------------------------\n");
    fprintf(txtFile, "Total Transaksi   : %d\n", count);
    fprintf(txtFile, "Total Pemasukan   : Rp%.2f\n", totalMasuk);
    fprintf(txtFile, "Total Pengeluaran : Rp%.2f\n", totalKeluar);
    fprintf(txtFile, "Saldo Akhir       : Rp%.2f\n", totalMasuk - totalKeluar);
    fprintf(txtFile, "=========================================================\n");

    printf("[SUKSES] Berhasil mengekspor laporan ke %s!\n", filenameTeks);

    fclose(binFile);
    fclose(txtFile);
}

void hitungSaldoTotal(const char *filename, double *totalPemasukan, double *totalPengeluaran) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "rb");
    
    // Inisialisasi total pemasukan dan pengeluaran ke 0
    *totalPemasukan = 0;
    *totalPengeluaran = 0;

    if (!file) {
        printf("Belum ada data keuangan atau file belum dibuat.\n");
        return;
    }

    // Baca data transaksi satu per satu
    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, file) == 1) {
        // Cek jika tipe adalah pemasukan (tidak sensitif huruf besar/kecil)
        if (strcasecmp(transaksi.tipe, "Pemasukan") == 0 || strstr(transaksi.tipe, "masuk") != NULL) {
            *totalPemasukan += transaksi.nominal;
        } 
        // Cek jika tipe adalah pengeluaran (tidak sensitif huruf besar/kecil)
        else if (strcasecmp(transaksi.tipe, "Pengeluaran") == 0 || strstr(transaksi.tipe, "keluar") != NULL) {
            *totalPengeluaran += transaksi.nominal;
        }
    }

    fclose(file);
}

// Menghitung ID terakhir + 1 agar ID tetap otomatis
int dapatkanIdBerikutnya(const char *filename) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "rb");
    int maxId = 0;

    if (!file) {
        return 1;
    }

    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, file) == 1) {
        if (transaksi.id > maxId) {
            maxId = transaksi.id;
        }
    }
    fclose(file);
    return maxId + 1;
}

// Quick add (./program --tambah)
void quickAddTransaksi(const char *filename, const char *tanggal, const char *kategori, const char *tipe, double nominal) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "ab");
    if (!file) {
        printf("Gagal membuka file database!\n");
        return;
    }

    int idBaru = dapatkanIdBerikutnya(filename);

    // Memanipulasi struct menggunakan pointer (->) lewat fungsi isiDataTransaksi
    isiDataTransaksi(&transaksi, idBaru, tanggal, kategori, tipe, nominal);

    fwrite(&transaksi, sizeof(CatatanKeuangan), 1, file);
    fclose(file);

    printf("\n[SUKSES] Transaksi berhasil ditambahkan secara cepat!\n");
    printf("ID       : %d\n", transaksi.id);
    printf("Tanggal  : %s\n", transaksi.tanggal);
    printf("Kategori : %s\n", transaksi.kategori);
    printf("Tipe     : %s\n", transaksi.tipe);
    printf("Nominal  : Rp%.2f\n\n", transaksi.nominal);
}
