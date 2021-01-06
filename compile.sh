/opt/optee-qemu/toolchains/aarch64/bin/aarch64-linux-gnu-gcc main.c trusted_cr.c -o main -static -Wl,-Ttext-segment,0x40050000
cp main /opt/optee-qemu/build/shared_folder/main
