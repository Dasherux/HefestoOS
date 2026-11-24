(pt-BR)

# HefestoOS

HefestoOS é um sistema operacional com arquitetura x86_64 em desenvolvimento, criado por Dasherux em 2026 e feito em Assembly e puro freestanding C, futuramente com próprias bibliotecas.

Estou usando Netwide Assembler (NASM), LLVM Linker, e QEMU (x86_64) pra rodar e compilar o código! :) 

## Objetivos:
- [x] Bootloader + Stage 2 com entrada ao modo de 64 Bits (o básico né);
- [x] Kernel com um linker pra C;
- [x] Drivers essenciais (teclado e tela);
- [x] Shell básico e funcional;
- [ ] Gerenciamento de memória;
- [ ] Interrupções e assincronia;
- [ ] Kernel multitarefa;
- [ ] Sistema de Arquivos (talvez varie dependendo da versão, mas o objetivo é FAT32);
- [ ] Self-hosting;
- [ ] UI gráfica + Mouse;
- [ ] Conexão à internet;

Pulls são bem-vindas! Vibecode é permitido se tiver uma boa e clara explicação do código e não ferrar com tudo futuramente. Agradeço se colocar comentários, preferência em idiomas com alfabeto latino (inglês, português, espanhol, alemão e por aí vai).

2026-2027 Dasherux.
