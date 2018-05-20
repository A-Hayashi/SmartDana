


//PC5 A/BB  H:RAM-A選択 L:RAM-B選択
//PC4 SE    H:内部バッファRAM手動切り替え L:内部バッファRAM自動切り替え
//PC3 A3    内部バッファRAMアドレス(MSB)
//PC2 A2    内部バッファRAMアドレス
//PC1 A1    内部バッファRAMアドレス
//PC0 A0    内部バッファRAMアドレス(LSB)
//
//PB4 ALE   H:アドレスラッチ有効 L:アドレスラッチ無効
//PB3 WE    H:内部バッファRAMへ書き込み L:内部バッファRAMへ書き込まない
//PB2 CLK   立ち上がりエッジでデータ取り込み
//PB1 DR    H:green L:消灯nnnnn
//PB0 DG    H:red L:消灯

#define SE  7
#define AB  8
#define A3  9
#define A2  10
#define A1  11
#define A0  12
#define DG  2
#define CLK  3
#define WE  4
#define DR  5
#define ALE 6


const unsigned int font_tbl_r[2][16] =
{
  {
    0x0000, 0x2040, 0x2844, 0x2BFC, 0xA844, 0xA844, 0xA044, 0xA7FE,
    0xA040, 0xA040, 0x20A0, 0x20A0, 0x2110, 0x2208, 0x2406, 0x0000
  },
  {
    0x0000, 0x6044, 0x37FC, 0x0040, 0x07FC, 0x0444, 0xE444, 0x27FC,
    0x2140, 0x2260, 0x2450, 0x2848, 0x2044, 0x5000, 0x8FFE, 0x0000
  }
};

const unsigned int font_tbl_g[2][16] =
{
  {
    0x0000, 0x2040, 0x2844, 0x2BFC, 0xA844, 0xA844, 0xA044, 0xA7FE,
    0xA040, 0xA040, 0x20A0, 0x20A0, 0x2110, 0x2208, 0x2406, 0x0000
  },
  {
    0x0000, 0x6044, 0x37FC, 0x0040, 0x07FC, 0x0444, 0xE444, 0x27FC,
    0x2140, 0x2260, 0x2450, 0x2848, 0x2044, 0x5000, 0x8FFE, 0x0000
  }
};

void send_data(unsigned char iaddr_y, unsigned long *idata_r, unsigned long *idata_g);

void send_data(unsigned char iaddr_y, unsigned long *idata_r, unsigned long *idata_g)
{
  //  volatile int i;
  volatile int j;
  unsigned long red;
  unsigned long green;

  red = *idata_r;
  green = *idata_g;

  for (j = 0; j < 32; j++) {
    digitalWrite(CLK, LOW);
    if ((red & 0x80000000UL) != 0) {    //MSBから送信していく
      digitalWrite(DR, HIGH);
    } else {
      digitalWrite(DR, LOW);
    }

    if ((green & 0x80000000UL) != 0) {  //MSBから送信していく
      digitalWrite(DG, HIGH);
    } else {
      digitalWrite(DG, LOW);
    }
    red = red << 1;       //次の列へ
    green = green << 1;   //次の列へ
    digitalWrite(CLK, HIGH);
  }

  //書き込むRAMのアドレスをセットする
  digitalWrite(A0, (iaddr_y >> 0) & 0x01);
  digitalWrite(A1, (iaddr_y >> 1) & 0x01);
  digitalWrite(A2, (iaddr_y >> 2) & 0x01);
  digitalWrite(A3, (iaddr_y >> 3) & 0x01);
  digitalWrite(WE, HIGH);

  //内部バッファRAMへ書き込み
  digitalWrite(ALE, HIGH);

  //クリア
  digitalWrite(WE, LOW);
  digitalWrite(ALE, LOW);
}

void setup() {
  pinMode(AB, OUTPUT);
  pinMode(SE, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DR, OUTPUT);
  pinMode(DG, OUTPUT);
  // put your setup code here, to run once:
  delay(100);
}

unsigned int disp_buf_r[2][16];
unsigned int disp_buf_g[2][16];

void loop() {
  int i;
  unsigned long red;
  unsigned long green;

  Serial.println("aaa");

  for (i = 0; i < 16; i++) {
    disp_buf_r[0][i] = pgm_read_word(&font_tbl_r[0][i]);
    disp_buf_r[1][i] = pgm_read_word(&font_tbl_r[1][i]);
    disp_buf_g[0][i] = pgm_read_word(&font_tbl_g[0][i]);
    disp_buf_g[1][i] = pgm_read_word(&font_tbl_g[1][i]);
  }
  for (i = 0; i < 16; i++) {
    red = disp_buf_r[0][i] << 16 | disp_buf_r[1][i];    //16bit+16bit=32bit(1行)
    green = disp_buf_g[0][i] << 16 | disp_buf_g[1][i];  //16bit+16bit=32bit(1行)
    send_data(i, &red, &green);                         //1行描画
  }
}
