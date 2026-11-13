#include "../include/fitur.h"

void tambahTransaksi(const char *filename) {
    CatatanKeuangan transaksi;
    FILE *file = fopen(filename, "ab"); // Mode append binary
    if (!file) {
        printf("Gagal membuka file database!\n");
        return;
    }

    printf("\n=== TAMBAH TRANSAKSI BARU ===\n");
    printf("Masukkan ID (angka): ");
    scanf("%d", &transaksi.id);
    printf("Masukkan Tanggal (YYYY-MM-DD): ");
    scanf("%s", transaksi.tanggal);
    printf("Masukkan Kategori (cth: Makanan/Gaji): ");
    scanf("%s", transaksi.kategori);
    printf("Masukkan Tipe (Pemasukan/Pengeluaran): ");
    scanf("%s", transaksi.tipe);
    printf("Masukkan Nominal: ");
    scanf("%lf", &transaksi.nominal);

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

    fprintf(txtFile, "========================================\n");
    fprintf(txtFile, "      LAPORAN KEUANGAN BULANAN          \n");
    fprintf(txtFile, "========================================\n");
    
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