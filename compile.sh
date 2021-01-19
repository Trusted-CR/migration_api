/opt/optee-qemu/toolchains/aarch64/bin/aarch64-linux-gnu-gcc -O0  main.c trusted_cr.c -o main -static -Wl,-Ttext-segment,0x40050000
cp -rf main /opt/optee-qemu/build/shared_folder/main
cp -rf main /opt/optee-hikey/out-br/target/root/shared/main
