# 🕹️ Volante Virtual com OpenCV e Controle de Teclas

Este projeto implementa um **volante virtual** usando a câmera do computador e a biblioteca **OpenCV**, permitindo controlar um veículo em jogos por meio da detecção de **cores (amarelo e magenta)**. As teclas direcionais, freio (↓) e nitro (espaço) são acionadas automaticamente conforme os movimentos dos marcadores coloridos.

## 📷 Demonstração Visual

O sistema detecta dois marcadores coloridos:
- **Amarelo** e **Magenta**: usados para determinar o ângulo de direção.
- A posição média entre eles define ações como **frear** ou ativar o **nitro**.

## 🎮 Controles Mapeados

| Ação            | Detecção                                                  | Tecla Simulada |
|------------------|-----------------------------------------------------------|----------------|
| Virar à esquerda | Ângulo entre marcadores < -30°                            | ⬅️ `VK_LEFT`   |
| Virar à direita  | Ângulo entre marcadores > 30°                             | ➡️ `VK_RIGHT`  |
| Frear            | Marcadores na região central da tela                     | ⬇️ `VK_DOWN`   |
| Nitro (NOS)      | Marcadores nas bordas inferiores (esquerda ou direita)   | ␣ `VK_SPACE`   |

## 🧪 Requisitos

- C++17 ou superior
- OpenCV 4.x
- Sistema **Windows** (usa `SendInput` e `XInput`)
- Webcam funcional

## 🧩 Estrutura do Projeto

- `Menu`: Interface de seleção para iniciar ou sair do sistema.
- `controlDirection`: Gera comandos de direção com base no ângulo dos marcadores.
- `controlBrakeNitro`: Ativa freio ou nitro baseado na posição dos marcadores.
- `sendKey`: Função que simula pressionamento de teclas via API do Windows.

## 📌 Observações

- O sistema não depende de mouse ou teclado durante o jogo.
- Ideal para jogos de corrida com suporte a teclado.
- Ajuste as faixas de cor (HSV) conforme a iluminação do ambiente.

## 📄 Licença

Este projeto é livre para fins educacionais e pessoais. Modificações são bem-vindas!
