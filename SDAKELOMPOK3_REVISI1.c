#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definisi konstanta untuk batasan program
#define MAX_TUGAS 100       // Maksimal jumlah tugas dalam antrian
#define MAX_SUBTUGAS 10     // Maksimal sub-tugas per tugas utama
#define RED     "\x1B[31m"  // Kode warna merah untuk output
#define GREEN   "\x1B[32m"  // Kode warna hijau untuk output
#define YELLOW  "\x1B[33m"  // Kode warna kuning untuk output
#define BLUE    "\x1B[34m"  // Kode warna biru untuk output
#define RESET   "\x1B[0m"   // Kode reset warna output

// Struktur untuk menyimpan data sub-tugas
typedef struct {
    char nama[100];     // Nama sub-tugas
    int kesulitan;      // Tingkat kesulitan sub-tugas (1-10)
} SubTugas;

// Struktur untuk menyimpan data tugas utama
typedef struct {
    char nama[100];     // Nama tugas utama
    char deadline[20];  // Deadline tugas (format DD/MM/YYYY)
    SubTugas subTugas[MAX_SUBTUGAS];  // Array sub-tugas
    int jumlahSubTugas; // Jumlah sub-tugas yang sudah dimasukkan
    int totalKesulitan; // Total nilai kesulitan semua sub-tugas
} Tugas;

// Struktur untuk antrian prioritas
typedef struct {
    Tugas data[MAX_TUGAS];  // Array untuk menyimpan tugas
    int size;               // Jumlah tugas saat ini dalam antrian
} PriorityQueue;

// Fungsi untuk inisialisasi antrian
void initQueue(PriorityQueue *queue) {
    queue->size = 0;  // Set ukuran awal ke 0 (antrian kosong)
}

// Fungsi untuk validasi format tanggal
int isValidDate(const char *date) {
    int day, month, year;
    // Parsing tanggal dari string
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3) {
        return 0;  // Format tidak sesuai
    }

    // Validasi nilai tanggal
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2023) {
        return 0;  // Tanggal tidak valid
    }

    return 1;  // Tanggal valid
}

// Fungsi untuk membandingkan dua deadline
int compareDeadline(const char *d1, const char *d2) {
    int day1, month1, year1;
    int day2, month2, year2;

    // Parsing tanggal pertama
    sscanf(d1, "%d/%d/%d", &day1, &month1, &year1);
    // Parsing tanggal kedua
    sscanf(d2, "%d/%d/%d", &day2, &month2, &year2);

    // Bandingkan tahun terlebih dahulu
    if (year1 != year2) return year1 - year2;
    // Jika tahun sama, bandingkan bulan
    if (month1 != month2) return month1 - month2;
    // Jika bulan sama, bandingkan hari
    return day1 - day2;
}

// Fungsi untuk menambahkan sub-tugas ke tugas utama
void tambahSubTugas(Tugas *tugas) {
    // Cek apakah sudah mencapai batas maksimal sub-tugas
    if (tugas->jumlahSubTugas >= MAX_SUBTUGAS) {
        printf(RED "Maksimal sub-tugas telah tercapai!\n" RESET);
        return;
    }

    SubTugas sub;
    printf("\nMasukkan nama sub-tugas: ");
    fgets(sub.nama, sizeof(sub.nama), stdin);
    // Hapus newline dari input
    sub.nama[strcspn(sub.nama, "\n")] = 0;

    printf("Masukkan tingkat kesulitan sub-tugas (1-10): ");
    scanf("%d", &sub.kesulitan);
    getchar();  // Membersihkan buffer

    // Validasi input kesulitan
    while (sub.kesulitan < 1 || sub.kesulitan > 10) {
        printf(RED "Nilai kesulitan harus antara 1-10: " RESET);
        scanf("%d", &sub.kesulitan);
        getchar();
    }

    // Tambahkan sub-tugas ke array
    tugas->subTugas[tugas->jumlahSubTugas] = sub;
    tugas->jumlahSubTugas++;
    // Tambahkan ke total kesulitan
    tugas->totalKesulitan += sub.kesulitan;

    printf(GREEN "Sub-tugas berhasil ditambahkan!\n" RESET);
}

// Fungsi untuk menampilkan daftar sub-tugas
void tampilkanSubTugas(Tugas tugas) {
    // Cek jika tidak ada sub-tugas
    if (tugas.jumlahSubTugas == 0) {
        printf("Tidak ada sub-tugas\n");
        return;
    }

    printf("\nDaftar Sub-Tugas:\n");
    printf("-----------------------------\n");
    // Loop melalui semua sub-tugas
    for (int i = 0; i < tugas.jumlahSubTugas; i++) {
        printf("%d. %s (Kesulitan: %d)\n",
               i+1,
               tugas.subTugas[i].nama,
               tugas.subTugas[i].kesulitan);
    }
    printf("-----------------------------\n");
    printf("Total Kesulitan: %d\n", tugas.totalKesulitan);
}

// Fungsi untuk menambahkan tugas ke antrian dengan prioritas
void enqueue(PriorityQueue *queue, Tugas newTugas) {
    // Cek jika antrian penuh
    if (queue->size >= MAX_TUGAS) {
        printf(RED "Queue penuh!\n" RESET);
        return;
    }

    // Hitung rata-rata kesulitan jika ada sub-tugas
    float avgKesulitan = newTugas.jumlahSubTugas > 0 ?
                        (float)newTugas.totalKesulitan / newTugas.jumlahSubTugas :
                        0;

    int pos = queue->size;

    // Cari posisi yang tepat untuk tugas baru
    while (pos > 0) {
        int cmp = compareDeadline(newTugas.deadline, queue->data[pos - 1].deadline);

        // Jika deadline lebih awal
        if (cmp < 0) {
            queue->data[pos] = queue->data[pos - 1];
        }
        // Jika deadline sama, bandingkan kesulitan
        else if (cmp == 0) {
            float avgKesulitanLama = queue->data[pos - 1].jumlahSubTugas > 0 ?
                                    (float)queue->data[pos - 1].totalKesulitan / queue->data[pos - 1].jumlahSubTugas :
                                    0;

            // Jika lebih sulit, geser ke atas
            if (avgKesulitan > avgKesulitanLama) {
                queue->data[pos] = queue->data[pos - 1];
            } else {
                break;
            }
        } else {
            break;
        }
        pos--;
    }

    // Sisipkan tugas baru di posisi yang ditemukan
    queue->data[pos] = newTugas;
    queue->size++;
}

// Fungsi untuk mengambil tugas dengan prioritas tertinggi
Tugas dequeue(PriorityQueue *queue) {
    // Buat tugas kosong untuk return value default
    Tugas kosong = {"", "", {{"", 0}}, 0, 0};

    // Cek jika antrian kosong
    if (queue->size == 0) {
        printf(RED "Queue kosong!\n" RESET);
        return kosong;
    }

    // Ambil tugas pertama (prioritas tertinggi)
    Tugas tugas = queue->data[0];

    // Geser semua elemen ke kiri
    for (int i = 0; i < queue->size - 1; i++) {
        queue->data[i] = queue->data[i + 1];
    }

    queue->size--;
    return tugas;
}

// Fungsi untuk menampilkan seluruh antrian
void displayQueue(PriorityQueue *queue) {
    // Cek jika antrian kosong
    if (queue->size == 0) {
        printf("\nTidak ada tugas dalam antrian.\n");
        return;
    }

    printf("\nDaftar Tugas (Diurutkan dari Prioritas Tertinggi):\n");
    printf("------------------------------------------------------------------\n");
    printf("No. | Nama Tugas           | Deadline     | Sub-Tugas | Avg Kesulitan\n");
    printf("------------------------------------------------------------------\n");

    // Loop melalui semua tugas
    for (int i = 0; i < queue->size; i++) {
        // Hitung rata-rata kesulitan
        float avgKesulitan = queue->data[i].jumlahSubTugas > 0 ?
                            (float)queue->data[i].totalKesulitan / queue->data[i].jumlahSubTugas :
                            0;

        // Tampilkan informasi tugas
        printf("%-3d | %-20s | %-12s | %-9d | %.1f\n",
               i + 1,
               queue->data[i].nama,
               queue->data[i].deadline,
               queue->data[i].jumlahSubTugas,
               avgKesulitan);
    }

    printf("------------------------------------------------------------------\n");
}

// Fungsi untuk melihat detail tugas tertentu
void lihatDetailTugas(PriorityQueue *queue) {
    // Cek jika antrian kosong
    if (queue->size == 0) {
        printf("\nTidak ada tugas dalam antrian.\n");
        return;
    }

    // Tampilkan daftar tugas
    displayQueue(queue);
    int taskNum;
    printf("\nMasukkan nomor tugas untuk melihat detail: ");
    scanf("%d", &taskNum);
    getchar();

    // Validasi nomor tugas
    if (taskNum < 1 || taskNum > queue->size) {
        printf(RED "Nomor tugas tidak valid!\n" RESET);
        return;
    }

    // Ambil tugas yang dipilih
    Tugas tugas = queue->data[taskNum-1];
    printf("\nDetail Tugas:\n");
    printf("Nama      : %s\n", tugas.nama);
    printf("Deadline  : %s\n", tugas.deadline);
    printf("Jumlah Sub-Tugas: %d\n", tugas.jumlahSubTugas);
    printf("Total Kesulitan: %d\n", tugas.totalKesulitan);

    // Jika ada sub-tugas, tampilkan
    if (tugas.jumlahSubTugas > 0) {
        printf("Rata-rata Kesulitan: %.1f\n",
              (float)tugas.totalKesulitan / tugas.jumlahSubTugas);
        tampilkanSubTugas(tugas);
    }
}

int main() {
    PriorityQueue queue;
    initQueue(&queue);  // Inisialisasi antrian
    int choice;

    printf("===== SISTEM ANTRIAN TUGAS DENGAN SUB-TUGAS =====\n");

    // Loop menu utama
    do {
        printf("\nMenu Utama:\n");
        printf("1. Tambah Tugas\n");
        printf("2. Lihat Daftar Tugas\n");
        printf("3. Lihat Detail Tugas\n");
        printf("4. Proses Tugas Terdepan\n");
        printf("5. Keluar\n");
        printf("Pilihan Anda: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                Tugas newTugas;
                newTugas.jumlahSubTugas = 0;
                newTugas.totalKesulitan = 0;

                printf("\nMasukkan Nama Tugas: ");
                fgets(newTugas.nama, sizeof(newTugas.nama), stdin);
                newTugas.nama[strcspn(newTugas.nama, "\n")] = 0;

                printf("Masukkan Deadline (DD/MM/YYYY): ");
                scanf("%s", newTugas.deadline);
                getchar();

                // Validasi format tanggal
                while (!isValidDate(newTugas.deadline)) {
                    printf(RED "Format deadline tidak valid! Gunakan format DD/MM/YYYY\n" RESET);
                    printf("Masukkan Deadline (DD/MM/YYYY): ");
                    scanf("%s", newTugas.deadline);
                    getchar();
                }

                // Menu sub-tugas
                int subChoice;
                do {
                    printf("\nMenu Sub-Tugas:\n");
                    printf("1. Tambah Sub-Tugas\n");
                    printf("2. Selesai\n");
                    printf("Pilihan: ");
                    scanf("%d", &subChoice);
                    getchar();

                    switch (subChoice) {
                        case 1:
                            tambahSubTugas(&newTugas);
                            break;
                        case 2:
                            printf(GREEN "Menambahkan tugas utama...\n" RESET);
                            break;
                        default:
                            printf(RED "Pilihan tidak valid!\n" RESET);
                    }
                } while (subChoice != 2);

                enqueue(&queue, newTugas);
                printf(GREEN "Tugas berhasil ditambahkan ke antrian!\n" RESET);
                break;
            }

            case 2:
                displayQueue(&queue);
                break;

            case 3:
                lihatDetailTugas(&queue);
                break;

            case 4: {
                Tugas diproses = dequeue(&queue);
                if (strlen(diproses.nama) > 0) {
                    printf("\nTugas yang sedang diproses:\n");
                    printf("Nama      : %s\n", diproses.nama);
                    printf("Deadline  : %s\n", diproses.deadline);
                    printf("Total Kesulitan: %d\n", diproses.totalKesulitan);
                    tampilkanSubTugas(diproses);
                }
                break;
            }

            case 5:
                printf("\nTerima kasih! Program selesai.\n");
                break;

            default:
                printf(RED "Pilihan tidak valid. Coba lagi.\n" RESET);
        }

    } while (choice != 5);

    return 0;
}
