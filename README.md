**LINK VIDEO DEMO** = https://drive.google.com/file/d/1DAyfaTgxerWykIpAkXvHcAbzTsh797A7/view?usp=sharing


# BOUNCING BALL CATCHER ROS2 + OPENCV

jadi ini adalah projek game menangkap bola yang memantul di kamera. projek ini menggunakan Ros2 dan OpenCV

## CARA MENJALANKAN

Untuk menjalankan game, siapkan 2 terminal yang sedang berada di folder ros2_ws. Pastikan sebelumnya telah dijalankan colcon build untuk packagenya dan jangan lupa untuk source install/setup.bash

Di terminal 1 jalankan command ini :

ros2 launch bouncing_ball_catcher game.launch.py

ini akan menjalankan gamenya namun masih belum tertampil di layar.

Di terminal 2 jalankan command ini untuk menampilkannya :
s
ros2 run rqt_image_view rqt_image_view

seharusnya akan muncul webcam yang kita gunakan disertai dengan label score yang dimulai dari 0. Kemudian terdapat sejumlah bola yang bergerak random memantul memantul dinding layar.

## CARA MAIN

Cukup sederhana, kita siapkan objek berwarna hijau (karena sudah diprogram mendeteksi warna hijau) seperti tutup botol santri. Arahkan tutup botol ke kamera dan sesuaikan jaraknya agar program mendeteksinya sebagai objek. Jika sudah, tinggal kita arahkan objek itu agar masuk ke dalam bola / lingkaran yang bergerak tadi. JIka berhasil masuk dalam area bola selama 2 detik maka skor akarn bertambah.

## ARSITEKTUR

Game ini bekerja melalui dua node utama, yakni camera_node dan game_node. camera_node bertugas untuk menangkap frame atau gambar melalui video atau sebuah kamera. Kemudian frame tersebut dipublish ke sebuah topic yang bernama camera/image_raw. 

node yang kedua yakni game_node yang menjadi subsriber ke topic tersebut dan mengolahnya agar game berjalan sesuai fungsinya.

Paramater yang digunakan dalam game ini sebagai berikut

camera_node:
  ros__parameters:
    source: "0"
    fps: 10.0
    width: 320
    height: 240

game_node:
  ros__parameters:
    hsv_lower: [38, 130, 100]
    hsv_upper: [75, 255, 255]
    min_area: 300
    num_balls: 3
    hold_duration: 2.0
    ball_radius: 40
    ball_speed: 150.0

    
## KEPUTUSAN DESAIN
Sumber gambar didapat melalui webcam daripada menggunakan file video karena lebih interaktif.

Pemilihan objek berwarna hijau muda untuk mendeteksi objek

timer dua detik saat centroid objek masuk ke dalam radius bola menggunakan visual garis yang mengelilingi keliling bola dengan warna kuning.

## KENDALA

Tidak terlalu memahami cara kerja node, topic dan sebagainya. Solusinya dengan membaca ulang modul yang diberikan

Tidak hafal dan terbiasa dengan syntax OpenCV dan rlcpp. Solusinya mempelajari syntax lewat internet dan AI kemudian menerapkannya

VM terlalu berat untuk menjalankan webcam yang berkahir patah - patah. Solusinya menurunkan resolusi saat game dijalankan.

## BONUS
Tidak ada karena untuk keseluruhan projek masih perlu didalami lagi (belajar lagi) sebelum ditambahkan fitur lainnya.

