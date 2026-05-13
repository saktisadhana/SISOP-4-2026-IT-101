
# **Project V-Protocol: HSS Recovery**

Dalam kekacauan yang melanda **Stasiun Luar Angkasa Herta (HSS)**, seorang Stellaron Hunter bernama Silver Wolf telah menyusup dan menginfeksi sistem inti stasiun dengan virus berbahaya, menyebabkan seluruh infrastruktur digital lumpuh total. Madame Herta, pemilik jenius eksentrik dari stasiun tersebut, tidak tinggal diam. Ia mengaktifkan **V-Protocol** — sebuah rencana pemulihan darurat rahasia yang hanya bisa dijalankan oleh teknisi terpercaya.

Kamu, Trailblazer, adalah teknisi tersebut.

Sebelum memulai misi, pastikan kamu berada di dalam direktori `task-3` yang berisi struktur berikut:

```text
task-3/
├── kernel.sh       # Protokol Pembangun Jantung Sistem
├── herta.sh        # Protokol Sintesis Pertahanan
├── curioDisc.sh    # Protokol Manifestasi Media (ISO)
└── save.sh         # Protokol Pengamanan & Pembersihan Jejak
```

Waktunya membuktikan kalau Trailblazer jauh lebih tangguh daripada Silverwolf!

---

## A. Kernel Ignition _(20 Points)_

Silver Wolf telah menyebarkan virus yang meracuni _kernel_ lama stasiun, menyebabkan kegagalan sistem di seluruh sektor. Madame Herta memerintahkanmu untuk memasuki "Forge Room" dan membangun jantung baru bagi stasiun agar V-Protocol memiliki fondasi yang bersih untuk dijalankan. Tanpa jantung yang stabil, infiltrasi balik ini akan gagal sebelum dimulai.

Gunakan file `kernel.sh` untuk melakukan langkah-langkah berikut secara otomatis:

- Unduh _source code_ kernel Linux **v6.1.1** ke dalam `kernel.sh`
- Lakukan konfigurasi kernel menggunakan `defconfig` atau `x86_64_defconfig`.
- Lakukan kompilasi kernel menggunakan compiler bawaan sistem hingga menghasilkan file `bzImage`.
- Pindahkan `bzImage` ke direktori utama dan pastikan folder pengerjaan tadi segera dihancurkan untuk menghilangkan jejak dari pantauan Silver Wolf.

---

## B. Enclave Synthesis _(35 Points)_

Jantung stasiun sudah siap, namun sistem membutuhkan pertahanan yang tangguh agar tidak mudah ditembus kembali oleh serangan siber berikutnya. Madame Herta menginginkan V-Protocol memiliki lapis keamanan ganda: sebuah jalur akses darurat untuk teknisi stasiun dan sebuah "Secure Enclave" terenkripsi untuk menyimpan rahasia-rahasianya dari intipan Stellaron Hunter.

Gunakan file `herta.sh` untuk membangun lingkungan sistem berkas (_RootFS_) menggunakan BusyBox dengan ketentuan:

- Sediakan **Single Mode** sebagai jalur akses cepat tanpa password yang menampilkan _banner_ penyambutan `HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED.` saat berhasil masuk.
- Sediakan **Multi Mode** yang terlindungi oleh user `nous` dengan password `nous123`. Pastikan kredensial tersebut tersimpan dalam bentuk terenkripsi.
- Buatlah file `secret.txt` di direktori _home_ user tersebut yang berisi, `Herta's Secret: Silver Wolf actually sucks at osu!`.

---

## C. ISO Manifest _(15 Points)_

Seluruh komponen penyelamat sudah siap, namun kamu butuh media untuk menyebarkan V-Protocol ke terminal stasiun yang terkunci di berbagai sektor. Kamu harus menyatukan _kernel_ dan sistem pertahananmu ke dalam sebuah disk penyelamat yang bisa mem-_bypass_ protokol lockdown Silver Wolf melalui bantuan bootloader GRUB.

Gunakan file `curioDisc.sh` agar dapat menghasilkan file ISO bernama `aha` dengan dua menu pilihan saat _booting_:

- Opsi pertama untuk memuat sistem **Emergency Shell** (Single Mode).
- Opsi kedua untuk memuat sistem **HSS Secure Login** (Multi Mode).

---

## D. Verification & Infiltration _(15 Points)_

Saatnya melakukan pengujian lapangan untuk memastikan infiltrasi balik berjalan lancar. Kamu harus melakukan pengujian secara manual menggunakan emulator QEMU untuk membuktikan bahwa media penyelamat yang telah kamu buat mampu menembus protokol keamanan Silver Wolf.

Lakukan pengujian manual dengan kriteria sebagai berikut:

- Pastikan sistem dapat melakukan booting sempurna menggunakan file ISO yang dihasilkan sebelumnya.
- Pada mode Single User, sistem wajib menampilkan _banner_ `HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED.` dan langsung memberikan akses root yang ditandai dengan prompt **`/ #`**.
- Lakukan login pada mode Multi User dengan kredensial yang tepat (`nous` / `nous123`).
- Buktikan bahwa file rahasia Madame Herta di direktori home dapat diakses serta dibaca dengan benar.

---

## E. Archiving & Final Wipe _(15 Points)_

Misi selesai! Kontrol stasiun telah berhasil direbut kembali. Namun, misi ini belum benar-benar berakhir sampai kamu mengamankan seluruh komponen V-Protocol ke dalam brankas digital HSS dan menghapus semua sisa pengerjaan agar pihak musuh tidak bisa menganalisis metode infiltrasi yang kamu gunakan.

Gunakan file `save.sh` untuk melakukan prosedur final sebagai berikut:

- Amankan file `bzImage`, file Ramdisk (`.gz`), dan file ISO ke dalam satu file arsip ZIP dengan nama `v_protocol_HSS.zip`.
- Lakukan pembersihan total terhadap seluruh file sisa kompilasi dan pengerjaan sistem.
- Pastikan direktori akhir benar-benar bersih dan hanya menyisakan file arsip serta keempat file utama pengerjaanmu saja.

  ***

## **NOTES**

- Seluruh skrip (`kernel.sh`, `herta.sh`, `curioDisc.sh`, `save.sh`) harus berjalan secara **non-interaktif**.
- Pembersihan file sisa harus menggunakan logika yang presisi (seperti `extglob`) agar skrip utama tidak ikut terhapus.

---

# **Project V-Protocol: HSS Recovery**

In the chaos engulfing the **Herta Space Station (HSS)**, a Stellaron Hunter named Silver Wolf has infiltrated and infected the station's core systems with a dangerous virus, causing the entire digital infrastructure to collapse. Madame Herta, the eccentric genius owner of the station, refuses to stand idle. She activates **V-Protocol** — a secret emergency recovery plan that can only be executed by a trusted technician.

You, Trailblazer, are that technician.

Before starting the mission, make sure you are inside the `task-3` directory which contains the following structure:

```text
task-3/
├── kernel.sh       # System Core Builder Protocol
├── herta.sh        # Defense Synthesis Protocol
├── curioDisc.sh    # Media Manifestation Protocol (ISO)
└── save.sh         # Securing & Evidence Wiping Protocol
```

Time to prove that Trailblazer is far better than Silver Wolf!

---

## A. Kernel Ignition _(20 Points)_

Silver Wolf has spread a virus that poisoned the station's old kernel, causing system failures across every sector. Madame Herta orders you to enter the "Forge Room" and build a new heart for the station so that V-Protocol has a clean foundation to run on. Without a stable core, this counter-infiltration will fail before it even begins.

Use the `kernel.sh` file to automatically perform the following steps:

- Download the Linux kernel **v6.1.1** source code into a working folder.
- Configure the kernel using `defconfig` or `x86_64_defconfig`.
- Compile the kernel using the system's default compiler until it produces the `bzImage` file.
- Move `bzImage` to the main directory and immediately destroy the working folder to erase all traces from Silver Wolf's surveillance.

---

## B. Enclave Synthesis _(35 Points)_

The station's heart is ready, but the system needs robust defenses to prevent being easily breached again by the next cyber attack. Madame Herta wants V-Protocol to have a dual-layer security system: an emergency access path for station technicians and an encrypted "Secure Enclave" to store her secrets away from Stellaron Hunter's prying eyes.

Use the `herta.sh` file to build a Root File System (RootFS) environment using BusyBox with the following requirements:

- Provide **Single Mode** as a quick access path without a password that displays the welcome banner `HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED.` upon successful entry.
- Provide **Multi Mode** protected by user `nous` with password `nous123`. Ensure the credentials are stored in encrypted form.
- Create a `secret.txt` file in that user's home directory containing, `Herta's Secret: Silver Wolf actually sucks at osu!`.

---

## C. ISO Manifest _(15 Points)_

All rescue components are ready, but you need media to deploy V-Protocol to the locked station terminals across various sectors. You must combine your kernel and defense system into a rescue disk image that can bypass Silver Wolf's lockdown protocol with the help of the GRUB bootloader.

Use the `curioDisc.sh` file to generate an ISO file named `aha` with two boot menu options:

- The first option to load the **Emergency Shell** system (Single Mode).
- The second option to load the **HSS Secure Login** system (Multi Mode).

---

## D. Verification & Infiltration _(15 Points)_

It's time for field testing to ensure the counter-infiltration runs smoothly. You must perform manual testing using the QEMU emulator to prove that the rescue media you have created is capable of breaching Silver Wolf's security protocols.

Perform manual testing with the following criteria:

- Ensure the system can boot successfully using the generated ISO file.
- In Single User mode, the system must display the banner `HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED.` and grant immediate root access indicated by the **`/ #`** prompt.
- Log in to Multi User mode with the correct credentials (`nous` / `nous123`).
- Prove that Madame Herta's secret file in the home directory can be accessed and read correctly.

---

## E. Archiving & Final Wipe _(15 Points)_

Mission complete! Station control has been successfully reclaimed. However, the mission isn't truly over until you secure all V-Protocol components into the HSS digital vault and erase all remaining work so that the enemy cannot analyze the infiltration method you used.

Use the `save.sh` file to carry out the following final procedures:

- Secure the `bzImage` file, the Ramdisk file (`.gz`), and the ISO file into a single ZIP archive named `v_protocol_HSS.zip`.
- Perform a total cleanup of all remaining compilation and system work files.
- Ensure the final directory is completely clean, leaving only the archive file and your four main working script files.

---

## **NOTES**

- All scripts (`kernel.sh`, `herta.sh`, `curioDisc.sh`, `save.sh`) must run **non-interactively**.
- The cleanup of remaining files must use precise logic (such as **extglob**) to ensure the main scripts are not accidentally deleted.
