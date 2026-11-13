#ifndef FITUR_H
#define FITUR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char tanggal[15];     
    char kategori[30];    
    char tipe[15];       
    double nominal;
} CatatanKeuangan;

void tambahTransaksi(const char *filename);
void tampilkanRiwayat(const char *filename);
void cariTransaksi(const char *filename, const char *keyword);
void eksporKeTeks(const char *filenameBinary, const char *filenameTeks);
void hitungSaldoTotal(const char *filename, double *totalPemasukan, double *totalPengeluaran);

#endif