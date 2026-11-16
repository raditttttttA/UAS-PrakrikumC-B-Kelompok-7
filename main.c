#include "../include/fitur.h"

void tampilkanBantuan() {
    printf("\n=== BANTUAN PENGGUNAAN APLIKASI KEUANGAN ===\n");
    printf("1. Jalankan interaktif : ./program\n");
    printf("2. Lihat bantuan       : ./program --help\n");
}

int main(int argc, char *argv[]) {
    char *dbFile = "data/keuangan.bin";
    char *txtFile = "data/laporan_bulanan.txt";

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            tampilkanBantuan();
            return 0;
        } else if (strcmp(argv[1], "--cari") == 0 && argc > 2) {
            cariTransaksi(dbFile, argv[2]);
            return 0;
        }
    }

    int pilihan;
    double pemasukan, pengeluaran;

    do {
        printf("\n=== APLIKASI PENCATAT KEUANGAN PRIBADI ===\n");
        printf("1. Tambah Transaksi\n");
        printf("2. Lihat Riwayat Transaksi\n");
        printf("3. Cari Transaksi (Kategori/Tipe)\n");
        printf("4. Hitung Saldo Total\n");
        printf("5. Ekspor Laporan ke Teks (.txt)\n");
        printf("6. Keluar\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                tambahTransaksi(dbFile);
                break;
            case 2:
                tampilkanRiwayat(dbFile);
                break;
            case 3: {
                char keyword[30];
                printf("Masukkan Kategori/Tipe yang dicari: ");
                scanf("%s", keyword);
                cariTransaksi(dbFile, keyword);
                break;
            }
            case 4:
                hitungSaldoTotal(dbFile, &pemasukan, &pengeluaran);
                printf("\nTotal Pemasukan  : Rp%.2f\n", pemasukan);
                printf("\nTotal Pengeluaran: Rp%.2f\n", pengeluaran);
                printf("Saldo Bersih     : Rp%.2f\n", pemasukan - pengeluaran);
                break;
            case 5:
                eksporKeTeks(dbFile, txtFile);
                break;
            case 6:
                printf("Terima kasih telah menggunakan aplikasi ini!\n");
                break;
            default:
                printf("Pilihan tidak valid!\n");
        }
    } while (pilihan != 6);

    return 0;
}