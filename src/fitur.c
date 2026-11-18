#include "../include/fitur.h"

// Fungsi internal untuk memastikan folder 'data/' ada secara otomatis
static void pastikanFolderAda() {
    MKDIR("data");
}

void tambahTransaksi(const char *filename) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "ab"); // Mode append binary
    if (!file) {
        printf("Gagal membuka file database!\n");
        return;
    }
    transaksi.id = dapatkanIdBerikutnya(filename);

    printf("\n=== TAMBAH TRANSAKSI BARU (Auto ID: #%d) ===\n", transaksi.id);    
    
    printf("Masukkan Tanggal (YYYY-MM-DD): ");
    scanf("%14s", transaksi.tanggal);
    
    int pilihanTipe;
    printf("Pilih Tipe Transaksi:\n  1. Pemasukan\n  2. Pengeluaran\nPilihan (1/2): ");
    if (scanf("%d", &pilihanTipe) != 1) {
        printf("[ERROR] Input tidak valid!\n");
        while (getchar() != '\n'); // Bersihkan buffer
        fclose(file);
        return;
    }
    
    if (pilihanTipe == 1) {
        strcpy(transaksi.tipe, "Pemasukan");
    } else {
        strcpy(transaksi.tipe, "Pengeluaran");
    }

    printf("Masukkan Kategori (cth: Makanan/Gaji): ");
    scanf("%29s", transaksi.kategori);
    
    printf("Masukkan Nominal (Rp): ");
    if (scanf("%lf", &transaksi.nominal) != 1 || transaksi.nominal <= 0) {
        printf("[ERROR] Nominal harus berupa angka positif!\n");
        while (getchar() != '\n'); // Bersihkan buffer
        fclose(file);
        return;
    }

    fwrite(&transaksi, sizeof(CatatanKeuangan), 1, file);
    fclose(file);
    printf("Berhasil menyimpan transaksi!\n");
}

void tampilkanRiwayat(const char *filename) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "rb"); 
    if (!file) {
        printf("\nBelum ada data transaksi atau file belum dibuat.\n");
        return;
    }

    printf("\n=========================================================\n");
    printf(" ID | Tanggal    | Kategori       | Tipe        | Nominal   \n");
    printf("=========================================================\n");
    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, file) == 1) {
        printf(" %-3d| %-10s | %-14s | %-11s | Rp%.2f\n", 
               transaksi.id, transaksi.tanggal, transaksi.kategori, transaksi.tipe, transaksi.nominal);
    }
    printf("=========================================================\n");
    fclose(file);
}

void cariTransaksi(const char *filename, const char *keyword) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "rb");
    int ditemukan = 0;

    if (!file) {
        printf("Database tidak ditemukan.\n");
        return;
    }

    printf("\n=== HASIL PENCARIAN: '%s' ===\n", keyword);
    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, file) == 1) {
        if (strcmp(transaksi.kategori, keyword) == 0 || strcmp(transaksi.tipe, keyword) == 0) {
            printf("ID: %d | Tanggal: %s | %s | Rp%.2f\n", 
                   transaksi.id, transaksi.tanggal, transaksi.kategori, transaksi.nominal);
            ditemukan = 1;
        }
    }
    if (!ditemukan) printf("Tidak ada data yang cocok.\n");
    fclose(file);
}

void eksporKeTeks(const char *filenameBinary, const char *filenameTeks) {
    CatatanKeuangan transaksi;
    FILE *binFile = fopen(filenameBinary, "rb");
    FILE *txtFile = fopen(filenameTeks, "w");

    if (!binFile || !txtFile) {
        printf("Gagal membuka file untuk ekspor laporan.\n");
        if (binFile) fclose(binFile);
        if (txtFile) fclose(txtFile);
        return;
    }

    fprintf(txtFile, "=======================================\n");
    fprintf(txtFile, "      LAPORAN KEUANGAN BULANAN          \n");
    fprintf(txtFile, "=======================================\n");
    
    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, binFile) == 1) {
        fprintf(txtFile, "[%s] %s - %s: Rp%.2f (Tgl: %s)\n", 
                transaksi.tipe, transaksi.kategori, transaksi.tipe, transaksi.nominal, transaksi.tanggal);
    }

    fprintf(txtFile, "========================================\n");
    printf("Berhasil mengekspor laporan ke %s!\n", filenameTeks);

    fclose(binFile);
    fclose(txtFile);
}

void hitungSaldoTotal(const char *filename, double *totalPemasukan, double *totalPengeluaran) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "rb");
    
    *totalPemasukan = 0;
    *totalPengeluaran = 0;

    if (!file) return;

    while (fread(&transaksi, sizeof(CatatanKeuangan), 1, file) == 1) {
        if (strcmp(transaksi.tipe, "Pemasukan") == 0) {
            *totalPemasukan += transaksi.nominal;
        } else if (strcmp(transaksi.tipe, "Pengeluaran") == 0) {
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

    transaksi.id = dapatkanIdBerikutnya(filename);
    strncpy(transaksi.tanggal, tanggal, sizeof(transaksi.tanggal) - 1);
    transaksi.tanggal[sizeof(transaksi.tanggal) - 1] = '\0';
    strncpy(transaksi.kategori, kategori, sizeof(transaksi.kategori) - 1);
    transaksi.kategori[sizeof(transaksi.kategori) - 1] = '\0';
    strncpy(transaksi.tipe, tipe, sizeof(transaksi.tipe) - 1);
    transaksi.tipe[sizeof(transaksi.tipe) - 1] = '\0';
    transaksi.nominal = nominal;

    fwrite(&transaksi, sizeof(CatatanKeuangan), 1, file);
    fclose(file);

    printf("\n[SUKSES] Transaksi berhasil ditambahkan secara cepat!\n");
    printf("ID       : %d\n", transaksi.id);
    printf("Tanggal  : %s\n", transaksi.tanggal);
    printf("Kategori : %s\n", transaksi.kategori);
    printf("Tipe     : %s\n", transaksi.tipe);
    printf("Nominal  : Rp%.2f\n\n", transaksi.nominal);
}
