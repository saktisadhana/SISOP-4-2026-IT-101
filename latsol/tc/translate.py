import sys
import re

file_path = r"c:\Users\Sakti Sadhana\Project\SISOP\modul_4\latsol\tc\README.md"
with open(file_path, "r", encoding="utf-8") as f:
    content = f.read()

replacements = [
    (r"Praktikan diinstruksikan untuk menjalankan protokol keamanan darurat Stasiun Luar Angkasa Herta \(V-Protocol\) dengan membuat empat shell script yaitu `kernel.sh`, `herta.sh`, `curioDisc.sh`, dan `save.sh`\. Seluruh script ini harus bisa dijalankan di Linux/WSL2 secara non-interaktif dengan previlege root \(sudo\)\.",
     "Students are instructed to execute the Herta Space Station emergency security protocol (V-Protocol) by creating four shell scripts: `kernel.sh`, `herta.sh`, `curioDisc.sh`, and `save.sh`. All of these scripts must be executable in Linux/WSL2 non-interactively with root privileges (sudo)."),
    
    (r"Langkah pertama pada misi ini adalah membangun \*kernel\* Linux yang baru untuk sistem stasiun\. Praktikan diminta menggunakan `kernel.sh` untuk mengunduh, mengonfigurasi, dan mengkompilasi \*source code\* kernel Linux v6\.1\.1 menjadi sebuah file `bzImage`\.",
     "The first step in this mission is to build a new Linux *kernel* for the station system. Students are required to use `kernel.sh` to download, configure, and compile the Linux kernel v6.1.1 *source code* into a `bzImage` file."),

    (r"Pada bagian awal skrip `kernel.sh`, program memastikan sistem diperbarui dengan `apt update` dan menginstall semua \*dependencies\* dasar untuk proses build kernel Linux menggunakan `apt install`\. Penggunaan flag `-y` memastikan skrip berjalan non-interaktif tanpa perlu konfirmasi manual\. Baris `set -e` digunakan agar skrip langsung berhenti jika ada perintah yang gagal \(error handling\)\.",
     "At the beginning of the `kernel.sh` script, the program ensures the system is updated with `apt update` and installs all basic *dependencies* for the Linux kernel build process using `apt install`. The use of the `-y` flag ensures the script runs non-interactively without needing manual confirmation. The `set -e` line is used so that the script stops immediately if any command fails (error handling)."),
     
    (r"Selanjutnya, program memeriksa arsitektur perangkat yang sedang digunakan menggunakan perintah `uname -m`\. Apabila perangkat pengguna menggunakan arsitektur `aarch64` \(seperti Apple Silicon Mac\), program akan secara otomatis mengunduh _cross-compiler_ `gcc-x86-64-linux-gnu` dan mengatur \*environment variable\* `CROSS_COMPILER`\. Apabila di Windows \(Intel/AMD\), program akan menggunakan kompilator bawaan\.",
     "Next, the program checks the architecture of the device being used using the `uname -m` command. If the user's device uses the `aarch64` architecture (like an Apple Silicon Mac), the program will automatically download the `gcc-x86-64-linux-gnu` _cross-compiler_ and set the `CROSS_COMPILER` *environment variable*. If on Windows (Intel/AMD), the program will use the native compiler."),

    (r"Setelah environment siap, program mengunduh \*source code\* Kernel v6\.1\.1 dengan perintah `wget`\. Apabila gagal, maka akan mengembalikan pesan error\. Jika sukses, file diekstrak menggunakan perintah `tar -xf` dan masuk ke direktori tersebut\.",
     "Once the environment is ready, the program downloads the Kernel v6.1.1 *source code* using the `wget` command. If it fails, it will return an error message. If successful, the file is extracted using the `tar -xf` command and enters that directory."),

    (r"Setelah masuk ke dalam folder, proses konfigurasi kernel dilakukan menggunakan pengaturan default dari `x86_64_defconfig`\. Proses kompilasi kemudian dilakukan menggunakan semua inti CPU \(`-j\$\(nproc\)`\) untuk mempercepat proses pembuatan file `bzImage`\.",
     "After entering the folder, the kernel configuration process is carried out using the default settings from `x86_64_defconfig`. The compilation process is then performed using all CPU cores (`-j$(nproc)`) to speed up the creation of the `bzImage` file."),

    (r"Langkah terakhir dari skrip ini adalah memindahkan hasil kompilasi `bzImage` kembali ke folder utama dan menghapus semua jejak \(source code dan tar\.xz\) menggunakan perintah `rm -rf` agar ruang penyimpanan tetap bersih\.",
     "The final step of this script is to move the compiled `bzImage` back to the main folder and delete all traces (source code and tar.xz) using the `rm -rf` command so that the storage space remains clean."),

    (r"Pada tahapan ini praktikan menggunakan skrip `herta.sh` untuk merakit sistem berkas \(_Root File System_ / RootFS\) mandiri menggunakan BusyBox yang dilengkapi dengan 2 mode yaitu \*Single User\* tanpa password dan \*Multi User\* dengan otentikasi\.",
     "In this stage, students use the `herta.sh` script to assemble a standalone file system (_Root File System_ / RootFS) using BusyBox, which is equipped with 2 modes: *Single User* without a password and *Multi User* with authentication."),

    (r"Sama halnya dengan kernel, script ini diwajibkan untuk di-\*run\* dengan hak akses root\. Baris ini memeriksa apakah EUID \(Effective User ID\) dari _executor_ bernilai 0 \(Root\)\.",
     "Similarly to the kernel, this script is required to be *run* with root access privileges. This line checks whether the EUID (Effective User ID) of the _executor_ is 0 (Root)."),

    (r"Disini skrip mengunduh komponen krusial `busybox-static` yang akan bertindak sebagai \*swiss-army knife\* untuk mengeksekusi semua command _basic_ Linux \(`ls`, `cat`, dll\) pada OS yang kita rancang\. Lalu membuat direktori-direktori umum Linux\.",
     "Here, the script downloads the crucial `busybox-static` component which will act as a *swiss-army knife* to execute all _basic_ Linux commands (`ls`, `cat`, etc.) on the OS we are designing. Then it creates common Linux directories."),

    (r"Program menempatkan binary utama `busybox` dan kemudian membuat symbolic link \(menggunakan argumen `--install -s`\) ke semua binary perintah Linux yang disokong oleh BusyBox\. Sistem OS mandiri juga membutuhkan \*device nodes\* dari Host untuk dapat berkomunikasi dengan input dan layar, sehingga node null, console, tty, dan zero di-copy\.",
     "The program places the main `busybox` binary and then creates symbolic links (using the `--install -s` argument) to all Linux command binaries supported by BusyBox. The standalone OS system also requires *device nodes* from the Host to be able to communicate with inputs and screens, so the null, console, tty, and zero nodes are copied."),

    (r"File `/init` dibuat sebagai program pertama yang dijalankan oleh kernel saat proses \*booting\* OS\. File `/init` ini melakukan _mount_ ke sistem virtual dasar\. Setelah itu membaca file `/proc/cmdline` yang membawa argumen dari GRUB\. Apabila di dalamnya terdapat string `single`, maka banner darurat dicetak ke layar lalu memberikan shell secara instan \(`/bin/sh`\) tanpa otentikasi\. Jika tidak, ia akan memanggil `/sbin/init` yang memulai layanan Multi-User mode\.",
     "The `/init` file is created as the first program executed by the kernel during the OS *booting* process. This `/init` file _mounts_ to the base virtual system. After that, it reads the `/proc/cmdline` file which carries arguments from GRUB. If it contains the string `single`, an emergency banner is printed to the screen and it instantly provides a shell (`/bin/sh`) without authentication. Otherwise, it will call `/sbin/init` which starts the Multi-User mode service."),

    (r"Bagian ini melakukan setup kredensial user untuk \*\*Multi User\*\* mode\. Semua file setup user Linux \(`passwd`, `shadow`, `group`\) ditambahkan\. Kredensial rahasia pengguna `nous` dengan password `nous123` diprogram menggunakan enkripsi command `chpasswd` yang berjalan di _chroot_ lingkungan RootFS kita\.",
     "This section sets up user credentials for **Multi User** mode. All Linux user setup files (`passwd`, `shadow`, `group`) are added. The secret credentials of the `nous` user with the password `nous123` are programmed using the encryption from the `chpasswd` command running in the _chroot_ of our RootFS environment."),

    (r"Kita membuat file rahasia Madame Herta `secret.txt` dengan isi teks sesuai instruksi\. Ownership dari folder pengguna tersebut diamankan untuk akun `nous`\.",
     "We create Madame Herta's secret file `secret.txt` with text content according to the instructions. The ownership of that user's folder is secured for the `nous` account."),

    (r"Langkah terakhir untuk sintesis _Enclave_ adalah mengatur sistem inisialisasi daemon `inittab` agar _spawn_ TTY \(antarmuka login\) pada saat multi-user mode dimulai\. Terakhir kita membungkus keseluruhan struktur folder menggunakan instruksi `cpio` menjadi RAM disk _filesystem_ dan di-kompresi dengan format gzip sehingga menghasilkan `RootFS.gz`\.",
     "The final step for the _Enclave_ synthesis is to configure the `inittab` daemon initialization system to _spawn_ a TTY (login interface) when multi-user mode starts. Finally, we package the entire folder structure using the `cpio` instruction into a RAM disk _filesystem_ and compress it with the gzip format, resulting in `RootFS.gz`."),

    (r"Pada bagian ini file ISO \*bootable\* dirangkai menggunakan script `curioDisc.sh`\. ISO ini akan menggabungkan modul `bzImage` \(dari bagian A\) dan sistem file `RootFS.gz` \(dari bagian B\) ke dalam \*bootloader\* GRUB\.",
     "In this section, a *bootable* ISO file is assembled using the `curioDisc.sh` script. This ISO will combine the `bzImage` module (from section A) and the `RootFS.gz` file system (from section B) into the GRUB *bootloader*."),

    (r"Sama dengan script sebelumnya, proses ISO ini membutuhkan privilese Root\. Command berikutnya juga memastikan program `grub-mkrescue`, `xorriso`, serta `mtools` terinstall dalam sistem untuk pembuatan ISO format bootable disk\.",
     "Similar to the previous script, this ISO process requires Root privileges. The next command also ensures that the `grub-mkrescue`, `xorriso`, and `mtools` programs are installed in the system for creating bootable disk format ISOs."),

    (r"Script akan terlebih dahulu melakukan pemeriksaan keamanan apakah file pendukung telah dibuat atau belum\. Apabila salah satu tidak ditemukan, program dihentikan menggunakan `exit 1`\.",
     "The script will first perform a security check to see whether the supporting files have been created or not. If one is not found, the program is stopped using `exit 1`."),

    (r"Setelah itu direktori dasar ISO yang diperlukan GRUB dirangkai, kemudian file Kernel beserta file Sistem disalin masuk ke dalam `iso/boot/`\.",
     "After that, the base ISO directories required by GRUB are assembled, then the Kernel file and the System file are copied into `iso/boot/`."),

    (r"Program secara dinamis merangkai berkas pengaturan `grub.cfg`\. Konfigurasi ini menyusun dua mode berbeda yang disyaratkan oleh Herta:\n1\. \*\*Emergency Shell \(Single Mode\)\*\*: Di mana ia mendelegasikan kata kunci `single` ke paramater \*booting\* kernel\. Variabel tersebut kemudian akan dievaluasi oleh sistem OS pada file `/init` untuk melepaskan Root Shell tanpa hambatan otentikasi\.\n2\. \*\*HSS Secure Login \(Multi Mode\)\*\*: Opsi default untuk melakukan login kredensial `nous`\.",
     "The program dynamically assembles the `grub.cfg` configuration file. This configuration sets up two different modes required by Herta:\n1. **Emergency Shell (Single Mode)**: Where it delegates the keyword `single` to the kernel *booting* parameters. This variable will then be evaluated by the OS system in the `/init` file to release a Root Shell without authentication hurdles.\n2. **HSS Secure Login (Multi Mode)**: The default option to perform the `nous` credential login."),

    (r"Tahap final adalah perintah `grub-mkrescue` yang akan meramu dan mengompilasi susunan direktori `iso/` di atas menjadi satu buah file \*image\* `\.iso` murni yakni `aha.iso`\.",
     "The final stage is the `grub-mkrescue` command which will mix and compile the `iso/` directory structure above into a single pure `.iso` *image* file, namely `aha.iso`."),

    (r"Tahap ini mengharuskan kita untuk menguji ketangguhan ISO `aha.iso` menggunakan alat virtualisasi emulator QEMU\.\nPengujian tidak memerlukan penambahan shell scripting namun hanya menjalankan perintah Linux dari Host OS:",
     "This stage requires us to test the resilience of the `aha.iso` ISO using the QEMU emulator virtualization tool.\nTesting does not require the addition of shell scripting but only running a Linux command from the Host OS:"),

    (r"Melalui langkah ini kita memverifikasi bahwa:\n1\. Bootloader menampilkan opsi GRUB dan kita bisa memilih dari menu\.\n2\. Saat menekan opsi \*\*Emergency Shell\*\*, mesin segera menghentikan proses \*boot\* ke mode reguler dan memunculkan notifikasi `HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED.` dilanjutkan ke akses shell root `/ #` secara \*direct\*\.\n3\. Saat memilih \*\*HSS Secure Login\*\*, sistem meminta \*credentials\*\. Apabila diinput nama pengguna `nous` serta sandinya `nous123`, kita berhasil diarahkan ke beranda profil tersebut\. \n4\. Ketika melayangkan perintah `cat /home/nous/secret.txt`, Herta's Secret pun muncul membeberkan kalimat: `Herta's Secret: Silver Wolf actually sucks at osu!`\.",
     "Through this step we verify that:\n1. The bootloader displays GRUB options and we can select from the menu.\n2. When selecting the **Emergency Shell** option, the machine immediately stops the regular *boot* process and displays the notification `HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED.` followed by *direct* access to the `/ #` root shell.\n3. When selecting **HSS Secure Login**, the system prompts for *credentials*. If the username `nous` and the password `nous123` are inputted, we are successfully directed to that profile's home directory. \n4. When executing the command `cat /home/nous/secret.txt`, Herta's Secret appears revealing the sentence: `Herta's Secret: Silver Wolf actually sucks at osu!`."),

    (r"Pada segmen penutup, semua hasil artefak dari program utama V-Protocol dikompilasi menjadi \*single payload\* di dalam skrip `save.sh`\. Seluruh residu lainnya diputuskan untuk dihanguskan\.",
     "In the closing segment, all artifact results from the main V-Protocol program are compiled into a *single payload* within the `save.sh` script. All other residues are decided to be destroyed."),

    (r"Selain mengecek kepemilikan Root \(EUID=0\), program menginstal utility paket kompresi data `zip`\.",
     "In addition to checking Root ownership (EUID=0), the program installs the `zip` data compression package utility."),

    (r"Protokol final mengecek silang kehadiran dari tiap-tiap entitas yakni jantung `bzImage`, disk virtual `RootFS.gz`, dan arsip virtual `aha.iso`\. Tidak satupun instrumen boleh tertinggal sebelum \*wipe\*\.",
     "The final protocol cross-checks the presence of each entity, namely the core `bzImage`, the virtual disk `RootFS.gz`, and the virtual archive `aha.iso`. Not a single instrument can be left behind before the *wipe*."),

    (r"Membungkus artefak hasil kerja tadi melalui format arsip terenkapsulasi \*ZIP\* ke dalam `v_protocol_HSS.zip`\.",
     "Wrapping the previous work artifacts via the encapsulated *ZIP* archive format into `v_protocol_HSS.zip`."),

    (r"Skrip ini menyalakan \*shell option extglob\* \(Extended Globbing\) menggunakan `shopt -s`\. Fungsinya adalah untuk mengizinkan instruksi negatif di dalam format pencocokan pola `!\( \.\.\. \)`\.\nDengan memanggil `rm -rf !\( \.\.\. \)`, sistem dieksekusi agar secara cerdas \*\*membumihanguskan semua direktori beserta file-file logis yang ada di lintasan root\*\* _KECUALI_ kerangka skrip utama: `kernel.sh`, `herta.sh`, `curioDisc.sh`, `save.sh`, dan produk target kita `v_protocol_HSS.zip`\. Pembersihan ekstrim ini memenuhi persyaratan tugas untuk menghapus jejak secara absolut dan presisi tanpa ikut memusnahkan script utamanya\.",
     "This script turns on the *shell option extglob* (Extended Globbing) using `shopt -s`. Its function is to allow negative instructions within the `!( ... )` pattern matching format.\nBy calling `rm -rf !( ... )`, the system is executed to intelligently **destroy all directories along with logical files located in the root path** _EXCEPT_ for the main script frameworks: `kernel.sh`, `herta.sh`, `curioDisc.sh`, `save.sh`, and our target product `v_protocol_HSS.zip`. This extreme cleanup fulfills the task requirements to erase traces absolutely and precisely without also destroying its main scripts.")
]

for pattern, replacement in replacements:
    content, count = re.subn(pattern, replacement, content)
    print(f"Replaced {count} instances.")

with open(file_path, "w", encoding="utf-8") as f:
    f.write(content)
