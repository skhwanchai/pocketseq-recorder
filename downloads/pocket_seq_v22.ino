// =============================================================
//  POCKET SEQ v2.2  —  บีต + เมโลดี้ 4 ชั้น + เครื่องดนตรี 10 แบบ + FX กีตาร์ + Swing / Chain / Mute
//  Raspberry Pi Pico 2 W + OLED 128x64 + ปุ่ม 4x4 + ปุ่ม 7 ตัว + ปุ่มหมุน 2 ตัว
//  + MAX98357A (แอมป์ I2S) + ลำโพง
// =============================================================
//
//  การต่อ MAX98357A:  Vin -> 3.3V  GND -> GND  BCLK -> GP20  LRC -> GP21  DIN -> GP22
//  ปุ่มหมุน A: CLK=GP0  DT=GP1  SW=GP2      ปุ่มหมุน B: CLK=GP14 DT=GP15 SW=GP3
//
//  ===== โหมดบีต (ปกติ) =====
//  ปุ่ม 4x4   หยุด = เปิด/ปิดช่องจังหวะของแทร็กที่เลือก | เล่น = เครื่องดนตรีเล่นสด (คอร์ดได้ 4 เสียง)
//  PLAY       เล่น / หยุด
//  SOUND      หยุด = เลือกแทร็ก KICK > SNARE > HAT > BASS | เล่น = เลือกเครื่องดนตรีเล่นสด
//  PTRN       เปลี่ยนแพตเทิร์น A > B > C > D
//  BPM        กดค้าง + S1 -10  S2 -1  S3 +1  S4 +10
//  FX         กดสั้น = CLEAN > OVERDRIVE > DISTORT > FUZZ > REVERB > DELAY (แบบเอฟเฟกต์กีตาร์)
//             กดค้าง 1 วิ = สลับ FX เฉพาะเครื่องดนตรี/เมโลดี้ (เริ่มต้น) <-> FX ทุกเสียงรวมกลอง
//             ปุ่มหมุน A โหมด FX AMT: OD/DIST/FUZZ = ความแตก  REVERB = ขนาดห้อง  DELAY = ความยาวเสียงสะท้อน
//  SELECT     กดสั้น = ล้างแทร็กที่เลือก | กดค้าง 1 วิ = ล้างทั้งแพตเทิร์น
//  REC        กด 1 ที = เข้าโหมดเมโลดี้ | กดค้าง 1 วิ = เซฟทันที
//
//  ===== Chain (โหมดบีต) =====
//  กด PTRN ค้าง + กด S1-S4 เรียงลำดับ (S1=A S2=B S3=C S4=D) แล้วปล่อย = ต่อแพตเทิร์นเป็นเพลง (สูงสุด 16)
//  กด PTRN สั้น ๆ ตอนมี chain = ยกเลิก chain   |   ไม่มี chain = เปลี่ยนแพตเทิร์นเหมือนเดิม
//
//  ===== Mute (ทุกโหมด) =====
//  กด SOUND ค้าง + ปุ่ม 4x4: S1-S4 = ปิด/เปิด KICK SNARE HAT BASS  |  S5-S8 = ปิด/เปิด ลูปเมโลดี้ L1-L4
//  (ค่า mute ไม่เซฟ ปิดเครื่องแล้วกลับมาดังทุกแทร็ก)
//
//  ===== โหมดเมโลดี้ (กด REC) — พิมพ์โน้ตทีละตัวเหมือนพิมพ์ข้อความ =====
//  เมโลดี้มี 4 ลูป (L1-L4) ลูปละ 16 ช่อง ทั้ง 4 ลูปเล่นพร้อมกันทับบีต วนทุกห้อง
//  เมโลดี้ชุดเดียวใช้ร่วมทุกแพตเทิร์น จนกว่าจะลบ
//  ปุ่ม 4x4   พิมพ์โน้ตลงช่องที่เคอร์เซอร์อยู่ แล้วเลื่อนไปช่องถัดไป (ได้ยินเสียงทันที)
//  BPM        ใส่ "เว้น" (--) แล้วเลื่อนไปช่องถัดไป
//  SELECT     กดสั้น = ลบตัวล่าสุด (ถอยกลับ 1 ช่อง) | กดค้าง 1 วิ = ล้างลูปนี้ทั้งลูป
//  PTRN       เลือกลูป L1 > L2 > L3 > L4
//  SOUND      เลือกเครื่องดนตรีของลูปที่เลือก (แต่ละลูปเลือกได้อิสระ)
//  REC        กดอีกครั้ง = กลับโหมดบีต | กดค้าง 1 วิ = เซฟ
//  พิมพ์ได้ทั้งตอนหยุดและตอนเพลงเล่น · ช่องที่เป็นโน้ตดังจนถึงช่องถัดไป ช่องเว้น = เงียบ
//
//  เครื่องดนตรี: PIANO PLUCK MARIMBA BELL CHIP SYNTH LEAD BRASS ORGAN FLUTE (+ DRUM+BASS เล่นสด)
//
//  ปุ่มหมุน A  หมุน = BPM | กด FX ค้าง + หมุน = ความแรง FX | กด BPM ค้าง + หมุน = SWING
//              กดปุ่มหมุน A ตามจังหวะ (แตะ 2-4 ครั้ง) = ตั้ง BPM จากการแตะ (TAP)
//  ปุ่มหมุน B  หมุน = ความดังรวม (VOL) | กด = เปิด/ปิด FX
//
//  v2.0: เซฟความดัง + FX ด้วย (chain ไม่เซฟ ปิดเครื่องแล้วหาย)
//  v2.1: ปลดล็อกจอค้างตอนเริ่มเครื่อง (หลังอัปโหลดไม่ต้องปิดเปิดใหม่)
//  v2.2: อัดเสียงเข้าคอมผ่านสาย USB (ใช้คู่กับไฟล์ PocketSeq-Recorder.html เปิดใน Chrome)
//  เซฟอัตโนมัติเมื่อหยุดเล่น 2 วินาที | รีเซ็ตทั้งหมด: กด SELECT ค้างไว้ขณะเสียบสาย USB
//  core 0 = ปุ่ม จอ เซฟ (setup/loop)   core 1 = เสียง นาฬิกาจังหวะ การอัด (setup1/loop1)
// =============================================================

#include <Wire.h>
#include <EEPROM.h>
#include <I2S.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------- ขาที่ต่อ ----------
const int rowPins[4] = {10, 11, 12, 13};   // R1-R4
const int colPins[4] = {9, 8, 7, 6};       // C1-C4

enum { B_PLAY, B_REC, B_SOUND, B_PTRN, B_BPM, B_FX, B_SELECT, NUM_BTN };
const int btnPins[NUM_BTN] = {16, 17, 18, 19, 26, 27, 28};

const int I2S_BCLK = 20;          // LRC ใช้ขาถัดไปอัตโนมัติ = GP21
const int I2S_DIN  = 22;
const int SAMPLE_RATE = 22050;    // เสียง 22,050 จุดต่อวินาที
I2S i2s(OUTPUT);

// ---------- ข้อมูลเพลง ----------
const int NUM_PTRN = 4;
const int NUM_TRK  = 4;
const int NUM_STEP = 16;
bool steps[NUM_PTRN][NUM_TRK][NUM_STEP];   // [แพตเทิร์น][แทร็ก][สเต็ป]

const char* trackNames[NUM_TRK] = {"KICK", "SNARE", "HAT", "BASS"};

// ---------- โน้ตเบส/เมโลดี้ (สเกล A ไมเนอร์เพนทาโทนิก กดมั่วยังไงก็เข้ากัน) ----------
const int NUM_NOTES = 12;
const char* noteNames[NUM_NOTES] = {"A2","C3","D3","E3","G3","A3","C4","D4","E4","G4","A4","C5"};
const int   noteSemis[NUM_NOTES] = { 0,   3,   5,   7,   10,  12,  15,  17,  19,  22,  24,  27 };
uint8_t bassNote[NUM_PTRN][NUM_STEP];     // โน้ตของแทร็ก BASS ในแต่ละสเต็ป

// ---------- เครื่องดนตรีเล่นสด ----------
enum { I_PIANO, I_PLUCK, I_MARIMBA, I_BELL, I_CHIP, I_SYNTH, I_LEAD, I_BRASS, I_ORGAN, I_FLUTE, I_DRUMS, NUM_INST };
const char* instNames[NUM_INST] = {"PIANO", "PLUCK", "MARIMBA", "BELL", "CHIP", "SYNTH", "LEAD", "BRASS", "ORGAN", "FLUTE", "DRUM+BASS"};
// เครื่องที่ดังค้างตามที่กด (ที่เหลือดังแล้วจางเอง)
bool isSustained(int inst) {
  return inst == I_ORGAN || inst == I_SYNTH || inst == I_CHIP || inst == I_FLUTE || inst == I_BRASS || inst == I_LEAD;
}
int liveInst = I_PIANO;
// 16 โน้ตบนปุ่ม 4x4 = คีย์ขาวของเปียโน เริ่มที่ C4 (โด กลาง)
const char* keyNames[16] = {"C4","D4","E4","F4","G4","A4","B4","C5",
                            "D5","E5","F5","G5","A5","B5","C6","D6"};
const int   keySemis[16] = { 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26 };

// ---------- เมโลดี้: 4 ลูป (ชั้นเสียง) x 16 ช่อง เล่นพร้อมกัน ใช้ร่วมทุกแพตเทิร์น ----------
const int MEL_LOOPS = 4;
const uint8_t NO_NOTE = 255;          // ยังไม่ได้พิมพ์
const uint8_t REST    = 254;          // เว้น
uint8_t melNote[MEL_LOOPS][NUM_STEP]; // โน้ต 0-15 (ตามปุ่ม S1-S16), REST หรือ NO_NOTE
volatile uint8_t melInst[MEL_LOOPS];  // เครื่องดนตรีของแต่ละลูป
const uint8_t DEFAULT_INST[MEL_LOOPS] = {0, 1, 3, 5};   // PIANO PLUCK BELL SYNTH
volatile bool melodyMode = false;     // อยู่ในโหมดเมโลดี้
volatile int  selLoop  = 0;           // ลูปที่เลือก (0-3)

// ---------- ความเร็ว ----------
const int BPM_MIN = 40;
const int BPM_MAX = 240;
const int bpmStepKeys[4] = {-10, -1, +1, +10};

// ค่าที่ทั้ง 2 core ใช้ร่วมกัน ต้องเป็น volatile
volatile int  bpm = 120;
volatile float masterVol = 0.7f;   // ความดังรวม (ปุ่มหมุน B)
volatile float fxAmount  = 0.5f;   // ความแรง FX 0.0 - 1.0 (กด FX ค้าง + หมุน A)
volatile int   swingPct  = 0;      // Swing 0-100% (กด BPM ค้าง + หมุน A)

// ---------- Chain: ต่อแพตเทิร์นเป็นเพลง (ไม่เซฟ) ----------
const int CHAIN_MAX = 16;
uint8_t chainSeq[CHAIN_MAX];
volatile int chainLen = 0;         // 0 = ไม่มี chain
volatile int chainPos = 0;

// ---------- Mute (ไม่เซฟ) ----------
volatile bool muteTrk[4]  = {false, false, false, false};   // KICK SNARE HAT BASS
volatile bool muteLoop[4] = {false, false, false, false};   // เมโลดี้ L1-L4
volatile int  ptrn = 0;
volatile bool playing = false;
// ---------- อัดเสียงส่งเข้าคอมผ่าน USB (v2.2) ----------
// core 1 เก็บเสียงลงบัฟเฟอร์วงกลม -> core 0 ส่งออกทางสาย USB เป็นแพ็กเก็ต
const uint32_t REC_N = 16384;                 // 16384 จุด = 0.74 วินาที
int16_t recBuf[REC_N];
volatile uint32_t recHead = 0, recTail = 0;   // core 1 เขียน head / core 0 อ่าน tail
volatile bool usbStream = false;              // กำลังส่งเสียงเข้าคอม
volatile bool usbArmed  = false;              // รอให้กด PLAY แล้วค่อยเริ่มส่ง
volatile bool fxKeysOnly = true;    // true = FX ใช้กับเครื่องดนตรี/เมโลดี้อย่างเดียว กลองเสียงปกติ (เหมือนเหยียบเอฟเฟกต์กีตาร์)
volatile int  fxMode = 0;          // 0=CLEAN 1=OVERDRIVE 2=DISTORT 3=FUZZ 4=REVERB 5=DELAY
int  lastFx = 1;                   // FX ที่จะเปิดเมื่อกดปุ่มหมุน B (เริ่มที่ OVERDRIVE)
enum { FX_OFF, FX_OD, FX_DIST, FX_FUZZ, FX_REVERB, FX_DELAY, NUM_FX };   // FX_OFF = CLEAN
const char* fxShort[NUM_FX] = {"CLN", "OD", "DIST", "FUZZ", "REV", "DLY"};   // บนป้ายมุมขวาล่าง
const char* fxLong[NUM_FX]  = {"CLEAN", "OVERDRIVE", "DISTORT", "FUZZ", "REVERB", "DELAY"};
// ป้ายตอนโหมด FX:KEYS  (\x0E = รูปโน้ตดนตรีในฟอนต์ของจอ)
const char* fxShortK[NUM_FX] = {"\x0E" "CLN", "\x0E" "OD", "\x0E" "DST", "\x0E" "FUZ", "\x0E" "REV", "\x0E" "DLY"};
volatile int  playStep = 0;
volatile bool trig[NUM_TRK];
volatile int  trigNote = -1;      // core 0 ขอให้เล่นโน้ตเบสตัวนี้ทันที
volatile bool trigOpenHat = false;

// คิวส่งข้อความ "กดโน้ต/ปล่อยโน้ต" จาก core 0 ไป core 1
enum { EV_ON = 1, EV_OFF = 2 };
const uint8_t EVQ_N = 32;
volatile uint32_t evq[EVQ_N];
volatile uint8_t  evHead = 0, evTail = 0;

void pushEvent(uint8_t type, uint8_t key, uint8_t inst) {
  uint8_t h = evHead;
  uint8_t nh = (h + 1) % EVQ_N;
  if (nh == evTail) return;                  // คิวเต็ม ทิ้งไป
  evq[h] = type | (key << 8) | (inst << 16);
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  evHead = nh;
}       // core 0 ขอให้ core 1 เล่นเสียงแทร็กนี้ทันที

int  trk  = 0;
bool recMode = false;
bool needRedraw = true;

// ---------- กันปุ่มเด้ง ----------
const unsigned long DEBOUNCE_MS = 25;
bool keyRaw[16], keyStable[16];  unsigned long keyTime[16];
bool btnRaw[NUM_BTN], btnStable[NUM_BTN];  unsigned long btnTime[NUM_BTN];

const unsigned long REPEAT_DELAY_MS = 400;
const unsigned long REPEAT_RATE_MS  = 70;
int heldBpmKey = -1;
unsigned long nextRepeatTime = 0;
bool wasBpmHeld = false;

const unsigned long SAVE_HOLD_MS = 1000;
unsigned long selectDownAt = 0;
bool selectHandled = false;
bool wasSelectHeld = false;

unsigned long recDownAt = 0;
bool recHandled = false;
bool wasRecHeld = false;

unsigned long fxDownAt = 0;
bool fxHandled = false;
bool wasFxHeld = false;

// ---------- ข้อความแจ้งเตือนบนจอ ----------
const char* flashMsg = nullptr;
unsigned long flashMsgUntil = 0;

void showMessage(const char* msg) {
  flashMsg = msg;
  flashMsgUntil = millis() + 1200;
  needRedraw = true;
}

// =============================================================
//  เซฟ / โหลด
// =============================================================
// ---------- เมโลดี้: ล้าง / เคอร์เซอร์ ----------
void clearMelodyLoop(int l) {
  for (int s = 0; s < NUM_STEP; s++) melNote[l][s] = NO_NOTE;
}
void clearAllMelodies() {
  for (int l = 0; l < MEL_LOOPS; l++) { clearMelodyLoop(l); melInst[l] = DEFAULT_INST[l]; }
}
int melCursor(int l) {                 // ตำแหน่งเคอร์เซอร์ = ช่องแรกที่ยังไม่ได้พิมพ์
  for (int s = 0; s < NUM_STEP; s++) if (melNote[l][s] == NO_NOTE) return s;
  return NUM_STEP;
}

struct SaveData {
  uint32_t magic;
  uint8_t  version;
  uint8_t  ptrn;
  uint16_t bpm;
  uint16_t steps[NUM_PTRN][NUM_TRK];
  uint8_t  notes[NUM_PTRN][NUM_STEP];   // เพิ่มใน v0.9
  uint8_t  melNote[MEL_LOOPS][NUM_STEP];   // v1.7: พิมพ์เมโลดี้ 4 ชั้น
  uint8_t  melInst[MEL_LOOPS];
  uint8_t  swing;                       // v1.9
  uint8_t  chainLen;                    // v1.9 (ตั้งแต่ v2.0 ไม่ใช้แล้ว)
  uint8_t  chain[CHAIN_MAX];
  uint8_t  vol;                         // v2.0: ความดัง 0-30 (x 0.05)
  uint8_t  fxMode;
  uint8_t  fxAmt;                       // ความแรง FX 0-20 (x 0.05)
  uint8_t  fxKeysOnly;
  uint8_t  lastFx;
};

const uint32_t SAVE_MAGIC   = 0x51455350;   // "PSEQ"
const uint8_t  SAVE_VERSION = 7;
const unsigned long AUTOSAVE_DELAY_MS = 2000;

bool dirty = false;
unsigned long lastChangeMs = 0;

void markDirty() {
  dirty = true;
  lastChangeMs = millis();
}

void saveNow() {
  SaveData d;
  d.magic   = SAVE_MAGIC;
  d.version = SAVE_VERSION;
  d.ptrn    = ptrn;
  d.bpm     = bpm;
  for (int p = 0; p < NUM_PTRN; p++)
    for (int t = 0; t < NUM_TRK; t++) {
      uint16_t bits = 0;
      for (int s = 0; s < NUM_STEP; s++) if (steps[p][t][s]) bits |= (1u << s);
      d.steps[p][t] = bits;
    }
  for (int p = 0; p < NUM_PTRN; p++)
    for (int s = 0; s < NUM_STEP; s++) d.notes[p][s] = bassNote[p][s];
  memcpy(d.melNote, melNote, sizeof(melNote));
  for (int l = 0; l < MEL_LOOPS; l++) d.melInst[l] = melInst[l];
  d.swing = swingPct;
  d.chainLen = 0;                      // chain ไม่เซฟ
  memset(d.chain, 0, sizeof(d.chain));
  d.vol        = (uint8_t)(masterVol * 20 + 0.5f);
  d.fxMode     = fxMode;
  d.fxAmt      = (uint8_t)(fxAmount * 20 + 0.5f);
  d.fxKeysOnly = fxKeysOnly ? 1 : 0;
  d.lastFx     = lastFx;
  EEPROM.put(0, d);
  if (EEPROM.commit()) { dirty = false; showMessage("SAVED"); }
  else                 { showMessage("SAVE ERR"); }
}

bool loadSaved() {
  SaveData d;
  EEPROM.get(0, d);
  if (d.magic != SAVE_MAGIC || (d.version < 1 || d.version > 7)) return false;   // 1 = เซฟจากรุ่นเก่า
  ptrn = d.ptrn % NUM_PTRN;
  bpm  = constrain((int)d.bpm, BPM_MIN, BPM_MAX);
  for (int p = 0; p < NUM_PTRN; p++)
    for (int t = 0; t < NUM_TRK; t++)
      for (int s = 0; s < NUM_STEP; s++)
        steps[p][t][s] = (d.steps[p][t] >> s) & 1;
  for (int p = 0; p < NUM_PTRN; p++)
    for (int s = 0; s < NUM_STEP; s++)
      bassNote[p][s] = (d.version >= 2) ? d.notes[p][s] % NUM_NOTES : 0;
  if (d.version >= 5) {
    memcpy(melNote, d.melNote, sizeof(melNote));
    for (int l = 0; l < MEL_LOOPS; l++) {
      melInst[l] = (d.melInst[l] < I_DRUMS) ? d.melInst[l] : DEFAULT_INST[l];
      for (int s = 0; s < NUM_STEP; s++)
        if (melNote[l][s] >= 16 && melNote[l][s] != REST) melNote[l][s] = NO_NOTE;
    }
  } else {
    clearAllMelodies();
  }
  if (d.version >= 6) swingPct = (d.swing <= 100) ? d.swing : 0;
  else                swingPct = 0;
  chainLen = 0;                         // chain ไม่เก็บข้ามการปิดเครื่อง
  if (d.version >= 7) {                 // v2.0: ความดัง + FX
    masterVol  = (d.vol <= 30) ? d.vol * 0.05f : 0.7f;
    fxMode     = (d.fxMode < NUM_FX) ? d.fxMode : FX_OFF;
    fxAmount   = (d.fxAmt <= 20) ? d.fxAmt * 0.05f : 0.5f;
    fxKeysOnly = d.fxKeysOnly != 0;
    lastFx     = (d.lastFx > FX_OFF && d.lastFx < NUM_FX) ? d.lastFx : FX_OD;
  }
  return true;
}

void loadDemo() {
  for (int p = 0; p < NUM_PTRN; p++)
    for (int t = 0; t < NUM_TRK; t++)
      for (int s = 0; s < NUM_STEP; s++) steps[p][t][s] = false;
  for (int s = 0; s < NUM_STEP; s += 4) steps[0][0][s] = true;   // KICK
  steps[0][1][4] = steps[0][1][12] = true;                        // SNARE
  for (int s = 0; s < NUM_STEP; s += 2) steps[0][2][s] = true;   // HAT
  steps[0][3][0] = steps[0][3][3] = steps[0][3][6] = steps[0][3][10] = true;  // BASS
  for (int p = 0; p < NUM_PTRN; p++)
    for (int s = 0; s < NUM_STEP; s++) bassNote[p][s] = 0;
  bassNote[0][6] = 3;  bassNote[0][10] = 2;   // ให้เบสตัวอย่างมีทำนองนิดหน่อย
  clearAllMelodies();
  ptrn = 0;
  bpm  = 120;
  masterVol = 0.7f;
  fxMode = FX_OFF;  fxAmount = 0.5f;  fxKeysOnly = true;  lastFx = FX_OD;
}

// =============================================================
//  ปุ่มหมุน (rotary encoder)
// =============================================================
const int encClk[2] = {0, 14};
const int encDt[2]  = {1, 15};
const int encSw[2]  = {2, 3};
const bool ENC_REVERSE[2] = {false, false};   // ถ้าหมุนแล้วทิศกลับด้าน เปลี่ยนเป็น true

volatile int  encDelta[2] = {0, 0};
volatile uint8_t encState[2] = {0, 0};
bool encSwRaw[2], encSwStable[2];  unsigned long encSwTime[2];
bool bpmKnobUsed = false;          // ระหว่างกด BPM ค้าง มีการหมุน A (ปรับ swing) ไหม
unsigned long tapTimes[4];         // แตะจังหวะ
int  tapCount = 0;
char knobMsg[16];

// ตารางถอดรหัสการหมุน (1 คลิก = 1 ครั้ง)
const uint8_t encTable[7][4] = {
  {0x0, 0x2, 0x4, 0x0}, {0x3, 0x0, 0x1, 0x10}, {0x3, 0x2, 0x0, 0x0},
  {0x3, 0x2, 0x1, 0x0}, {0x6, 0x0, 0x4, 0x0}, {0x6, 0x5, 0x0, 0x20},
  {0x6, 0x5, 0x4, 0x0}
};

bool justPressed(bool raw, bool &lastRaw, bool &stable, unsigned long &t);
void changeBpm(int delta);

void encUpdate(int i) {
  uint8_t pins = (digitalRead(encDt[i]) << 1) | digitalRead(encClk[i]);
  uint8_t st = encTable[encState[i] & 0x0F][pins];
  encState[i] = st;
  int dir = 0;
  if (st & 0x10) dir = 1;
  if (st & 0x20) dir = -1;
  if (ENC_REVERSE[i]) dir = -dir;
  encDelta[i] += dir;
}
void encIsrA() { encUpdate(0); }
void encIsrB() { encUpdate(1); }

int takeEncDelta(int i) {
  noInterrupts();
  int d = encDelta[i];
  encDelta[i] = 0;
  interrupts();
  return d;
}

const char* fxNameNow() { return fxLong[fxMode]; }

void handleKnobs() {
  // ----- ปุ่มหมุน A: BPM / (FX ค้าง) ความแรง FX / (BPM ค้าง) SWING -----
  int dA = takeEncDelta(0);
  if (dA != 0) {
    if (btnStable[B_FX]) {                         // กด FX ค้าง + หมุน
      float a = fxAmount + dA * 0.05f;
      if (a < 0) a = 0;
      if (a > 1) a = 1;
      fxAmount = a;
      fxHandled = true;                            // ปล่อย FX แล้วไม่ต้องเปลี่ยนเอฟเฟกต์
      markDirty();
      snprintf(knobMsg, sizeof(knobMsg), "FX %d%%", (int)(a * 100 + 0.5f));
    } else if (btnStable[B_BPM]) {                 // กด BPM ค้าง + หมุน
      int sw = swingPct + dA * 5;
      if (sw < 0) sw = 0;
      if (sw > 100) sw = 100;
      swingPct = sw;
      bpmKnobUsed = true;
      markDirty();
      snprintf(knobMsg, sizeof(knobMsg), "SWG %d%%", sw);
    } else {                                       // หมุนเฉย ๆ = BPM
      changeBpm(dA);
      snprintf(knobMsg, sizeof(knobMsg), "BPM %d", (int)bpm);
    }
    showMessage(knobMsg);
  }

  // ----- ปุ่มหมุน B: ความดังรวม -----
  int dB = takeEncDelta(1);
  if (dB != 0) {
    float v = masterVol + dB * 0.05f;
    if (v < 0) v = 0;
    if (v > 1.5f) v = 1.5f;
    masterVol = v;
    markDirty();
    snprintf(knobMsg, sizeof(knobMsg), "VOL %d%%", (int)(v * 100 / 1.5f + 0.5f));
    showMessage(knobMsg);
  }

  // ----- กดปุ่มหมุน -----
  for (int i = 0; i < 2; i++) {
    bool r = (digitalRead(encSw[i]) == LOW);
    if (!justPressed(r, encSwRaw[i], encSwStable[i], encSwTime[i])) continue;
    if (i == 0) {                                   // A: แตะจังหวะตั้ง BPM
      unsigned long now = millis();
      if (tapCount > 0 && now - tapTimes[tapCount - 1] > 2000) tapCount = 0;   // เว้นนานไป เริ่มนับใหม่
      if (tapCount == 4) { for (int j = 0; j < 3; j++) tapTimes[j] = tapTimes[j + 1]; tapCount = 3; }
      tapTimes[tapCount++] = now;
      if (tapCount >= 2) {
        unsigned long avg = (tapTimes[tapCount - 1] - tapTimes[0]) / (tapCount - 1);
        if (avg > 0) {
          bpm = constrain((int)(60000UL / avg), BPM_MIN, BPM_MAX);
          markDirty();
        }
        snprintf(knobMsg, sizeof(knobMsg), "TAP %d", (int)bpm);
      } else {
        snprintf(knobMsg, sizeof(knobMsg), "TAP...");
      }
      showMessage(knobMsg);
    } else {                                        // B: เปิด/ปิด FX ทันที
      if (fxMode != FX_OFF) { lastFx = fxMode; fxMode = FX_OFF; }
      else                  { fxMode = lastFx; }
      markDirty();
      showMessage(fxNameNow());
    }
  }
}

// =============================================================
//  อินพุต (core 0)
// =============================================================
bool justPressed(bool raw, bool &lastRaw, bool &stable, unsigned long &t) {
  unsigned long now = millis();
  if (raw != lastRaw) { lastRaw = raw; t = now; }
  if (now - t >= DEBOUNCE_MS && stable != raw) {
    stable = raw;
    return stable;
  }
  return false;
}

void readKeypad(bool raw[16]) {
  for (int c = 0; c < 4; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], LOW);
    delayMicroseconds(10);
    for (int r = 0; r < 4; r++) raw[r * 4 + c] = (digitalRead(rowPins[r]) == LOW);
    pinMode(colPins[c], INPUT);
  }
}

bool bpmHeld() { return btnStable[B_BPM]; }

void previewTrack(int t) { trig[t] = true; }   // ให้ core 1 เล่นเสียงนี้ทันที

void changeBpm(int delta) {
  int nb = constrain(bpm + delta, BPM_MIN, BPM_MAX);
  if (nb != bpm) { bpm = nb; markDirty(); }
  needRedraw = true;
}

// ล้างแทร็กเดียว
void clearTrack(int p, int t) {
  for (int s = 0; s < NUM_STEP; s++) {
    steps[p][t][s] = false;
    if (t == 3) bassNote[p][s] = 0;
  }
}

// ปุ่ม 4x4 ตอนกำลังเล่น = คีย์เล่นสด (ถ้าเปิด REC จะจดลงแพตเทิร์นด้วย)
void liveKey(int k) {
  if (liveInst != I_DRUMS) {                     // เปียโน / ออร์แกน / ระฆัง / ซินธ์
    pushEvent(EV_ON, k, liveInst);
    showMessage(keyNames[k]);
    return;
  }
  int s = playStep;
  const char* name;
  if (k < 4) {                                   // แถวบน = กลอง
    int t = (k == 3) ? 2 : k;                    // S4 = ไฮแฮตเปิด เก็บลงแทร็ก HAT
    if (k == 3) { trigOpenHat = true; name = "OPEN HAT"; }
    else        { trig[t] = true;     name = trackNames[t]; }
    if (recMode) { steps[ptrn][t][s] = true; markDirty(); }
  } else {                                       // 3 แถวล่าง = โน้ต
    int n = k - 4;
    trigNote = n;
    name = noteNames[n];
    if (recMode) { steps[ptrn][3][s] = true; bassNote[ptrn][s] = n; markDirty(); }
  }
  showMessage(name);
}

// ---------- ปุ่มที่ทำงานตอนปล่อยนิ้ว (เพราะกดค้างคู่กับปุ่มอื่นได้) ----------
bool ptrnUsed = false, wasPtrnHeld = false;     // ระหว่างกด PTRN ค้าง มีการกดปุ่ม 4x4 ไหม
bool soundUsed = false, wasSoundHeld = false;   // ระหว่างกด SOUND ค้าง มีการกดปุ่ม 4x4 ไหม
unsigned long soundDownAt = 0;
uint8_t chainTemp[CHAIN_MAX];
int  chainTempLen = 0;
char chainMsg[24];

bool mixerShown() {                    // หน้ามิกเซอร์ (กด SOUND ค้าง)
  return btnStable[B_SOUND] && (soundUsed || millis() - soundDownAt > 250);
}

// ---------- โหมดเมโลดี้: พิมพ์โน้ต ----------
char melMsg[16];
unsigned long previewOffAt = 0;      // ปิดเสียงลองฟังตอนเปลี่ยนเครื่องดนตรี

void melType(uint8_t v) {             // ใส่โน้ตหรือเว้นที่เคอร์เซอร์ แล้วเลื่อนไป
  int l = selLoop;
  int c = melCursor(l);
  if (c >= NUM_STEP) { showMessage("FULL 16/16"); return; }
  melNote[l][c] = v;
  markDirty();
  if (v == REST) showMessage("REST");
  else           showMessage(keyNames[v]);
}

void melBackspace() {                 // ลบตัวล่าสุด
  int l = selLoop;
  int c = melCursor(l);
  if (c == 0) { showMessage("EMPTY"); return; }
  melNote[l][c - 1] = NO_NOTE;
  markDirty();
  showMessage("DEL");
}

void soundAction() {
  if (melodyMode) {                // โหมดเมโลดี้ -> เครื่องดนตรีของลูปที่เลือก
    int i = melInst[selLoop];
    do { i = (i + 1) % NUM_INST; } while (i == I_DRUMS);
    melInst[selLoop] = i;
    markDirty();
    showMessage(instNames[i]);
    pushEvent(EV_ON, 7, i);        // ลองฟังสั้น ๆ (โน้ต C5)
    previewOffAt = millis() + 350;
  } else if (playing) {            // กำลังเล่น -> เปลี่ยนเครื่องดนตรีเล่นสด
    liveInst = (liveInst + 1) % NUM_INST;
    showMessage(instNames[liveInst]);
  } else {                         // หยุดอยู่ -> เปลี่ยนแทร็กที่จะแก้
    trk = (trk + 1) % NUM_TRK;
    previewTrack(trk);
  }
  needRedraw = true;
}

void onKey(int k) {
  // กด SOUND ค้าง + ปุ่ม = ปิด/เปิดเสียง (แถวบน = กลอง, แถว 2 = ลูปเมโลดี้)
  if (btnStable[B_SOUND]) {
    soundUsed = true;
    if (k < 4)      muteTrk[k] = !muteTrk[k];
    else if (k < 8) muteLoop[k - 4] = !muteLoop[k - 4];
    needRedraw = true;
    return;
  }

  // โหมดบีต: กด PTRN ค้าง + S1-S4 = ใส่แพตเทิร์นลง chain
  if (!melodyMode && btnStable[B_PTRN]) {
    ptrnUsed = true;
    if (k < 4 && chainTempLen < CHAIN_MAX) {
      chainTemp[chainTempLen++] = k;
      int n = 0;
      n += snprintf(chainMsg + n, sizeof(chainMsg) - n, "CH");
      int from = chainTempLen > 6 ? chainTempLen - 6 : 0;   // จอแคบ โชว์ 6 ตัวท้าย
      for (int i = from; i < chainTempLen && n < (int)sizeof(chainMsg) - 3; i++)
        n += snprintf(chainMsg + n, sizeof(chainMsg) - n, " %c", 'A' + chainTemp[i]);
      showMessage(chainMsg);
    }
    return;
  }

  // โหมดเมโลดี้: ปุ่ม 4x4 = พิมพ์โน้ต
  if (melodyMode) {
    pushEvent(EV_ON, k, melInst[selLoop]);       // ได้ยินเสียงทันที (ปล่อยปุ่ม = ปล่อยเสียง)
    melType(k);
    needRedraw = true;
    return;
  }

  if (bpmHeld()) {
    if (k < 4) {
      changeBpm(bpmStepKeys[k]);
      heldBpmKey = k;
      nextRepeatTime = millis() + REPEAT_DELAY_MS;
    }
    return;
  }

  if (playing) {                                 // กำลังเล่น -> เล่นสด
    liveKey(k);
    needRedraw = true;
    return;
  }

  // หยุดอยู่ -> เปิด/ปิดจังหวะของแทร็กที่เลือก
  steps[ptrn][trk][k] = !steps[ptrn][trk][k];
  if (steps[ptrn][trk][k]) {
    if (trk == 3) trigNote = bassNote[ptrn][k];  // ลองฟังโน้ตของสเต็ปนั้น
    else          previewTrack(trk);
  }
  markDirty();
  needRedraw = true;
}

void onButton(int b) {
  switch (b) {
    case B_PLAY:
      playing = !playing;
      if (!playing) digitalWrite(LED_BUILTIN, LOW);
      break;
    case B_REC:
      recDownAt = millis();            // ยังไม่ทำอะไร รอดูว่ากดสั้นหรือกดค้าง (ดูใน loop)
      recHandled = false;
      break;
    case B_SOUND:
      soundDownAt = millis();          // ทำงานตอนปล่อย (ดู loop) เผื่อกดค้างเพื่อ mute
      soundUsed = false;
      break;
    case B_PTRN:
      if (melodyMode) {                // โหมดเมโลดี้ -> เลือกลูป (ทันที)
        selLoop = (selLoop + 1) % MEL_LOOPS;
        snprintf(melMsg, sizeof(melMsg), "LOOP %d", selLoop + 1);
        showMessage(melMsg);
      } else {                         // โหมดบีต -> ทำงานตอนปล่อย เผื่อกดค้างตั้ง chain
        ptrnUsed = false;
        chainTempLen = 0;
      }
      break;
    case B_BPM:
      bpmKnobUsed = false;             // โหมดเมโลดี้: ใส่ "เว้น" ตอนปล่อย (ดู loop)
      break;
    case B_FX:
      fxDownAt = millis();             // รอดูว่ากดสั้นหรือกดค้าง (ดูใน loop)
      fxHandled = false;
      break;
    case B_SELECT:
      selectDownAt = millis();
      selectHandled = false;
      break;
  }
  needRedraw = true;
}

// =============================================================
//  หน้าจอ (core 0)
// =============================================================
void badge(int x, int y, const char* label, bool on) {
  int w = strlen(label) * 6 + 3;
  if (on) {
    display.fillRect(x - 1, y - 1, w, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else {
    display.drawRect(x - 1, y - 1, w, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(x + 1, y + 1);
  display.print(label);
  display.setTextColor(SSD1306_WHITE);
}

void drawBpmScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("SET TEMPO");
  display.setCursor(86, 0);
  display.print("SW "); display.print((int)swingPct); display.print('%');

  int b = bpm;
  display.setTextSize(3);
  int w = ((b >= 100) ? 3 : 2) * 18;
  display.setCursor((128 - w - 20) / 2, 16);
  display.print(b);
  display.setTextSize(1);
  display.setCursor((128 - w - 20) / 2 + w + 3, 30);
  display.print("bpm");

  const char* labels[4] = {"-10", "-1", "+1", "+10"};
  for (int i = 0; i < 4; i++) {
    int x = i * 32;
    if (keyStable[i]) {
      display.fillRect(x + 1, 48, 30, 15, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.drawRect(x + 1, 48, 30, 15, SSD1306_WHITE);
      display.setTextColor(SSD1306_WHITE);
    }
    int tw = strlen(labels[i]) * 6;
    display.setCursor(x + 1 + (30 - tw) / 2, 52);
    display.print(labels[i]);
  }
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

// หน้าจอโหมดเมโลดี้: 16 ช่องเหมือนบรรทัดข้อความ
// หน้ามิกเซอร์ (กด SOUND ค้าง): ทึบ = ดัง, กรอบ = ปิดเสียง
void drawMixerScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("MUTE  SOUND + KEY");
  const char* top[4] = {"KICK", "SNRE", "HAT", "BASS"};
  for (int r = 0; r < 2; r++) {
    for (int i = 0; i < 4; i++) {
      int x = i * 32, y = 14 + r * 22;
      bool muted = (r == 0) ? muteTrk[i] : muteLoop[i];
      if (!muted) { display.fillRect(x + 1, y, 30, 18, SSD1306_WHITE); display.setTextColor(SSD1306_BLACK); }
      else        { display.drawRect(x + 1, y, 30, 18, SSD1306_WHITE); display.drawLine(x + 1, y + 17, x + 30, y, SSD1306_WHITE); }
      display.setCursor(x + 4, y + 6);
      if (r == 0) display.print(top[i]);
      else { display.print(" L"); display.print(i + 1); }
      display.setTextColor(SSD1306_WHITE);
    }
  }
  display.setCursor(0, 57);
  display.print("S1-S4 DRUM  S5-S8 LOOP");
  display.display();
}

bool anyMuted() {
  for (int i = 0; i < 4; i++) if (muteTrk[i] || muteLoop[i]) return true;
  return false;
}

void drawMelodyScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  int l = selLoop;
  int cur = melCursor(l);
  int ps = playStep;

  // แถวบน: L2 + เครื่องดนตรี | จำนวนช่องที่พิมพ์
  display.fillRect(0, 0, 15, 9, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(2, 1);
  display.print('L'); display.print(l + 1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(19, 1);
  display.print(instNames[melInst[l]]);
  display.setCursor(cur >= 10 ? 98 : 104, 1);
  display.print(cur); display.print("/16");

  // 16 ช่อง: 2 แถว แถวละ 8
  for (int s = 0; s < NUM_STEP; s++) {
    int x = (s % 8) * 16;
    int y = 13 + (s / 8) * 12;
    uint8_t n = melNote[l][s];
    bool head = playing && s == ps;
    if (head) display.fillRect(x, y - 1, 15, 10, SSD1306_WHITE);
    display.setTextColor(head ? SSD1306_BLACK : SSD1306_WHITE);
    display.setCursor(x + 2, y);
    if (n < 16)          display.print(keyNames[n]);
    else if (n == REST)  display.print("--");
    else                 display.drawPixel(x + 7, y + 6, head ? SSD1306_BLACK : SSD1306_WHITE);   // ยังว่าง
    if (s == cur) display.drawFastHLine(x + 1, y + 9, 13, SSD1306_WHITE);                     // เคอร์เซอร์
  }
  display.setTextColor(SSD1306_WHITE);

  // ภาพรวม 4 ลูป: ทึบ = มีโน้ต, กรอบหนา = ลูปที่เลือก
  for (int i = 0; i < MEL_LOOPS; i++) {
    int x = i * 24;
    bool has = melCursor(i) > 0;
    if (has) { display.fillRect(x, 39, 21, 11, SSD1306_WHITE); display.setTextColor(SSD1306_BLACK); }
    else     { display.drawRect(x, 39, 21, 11, SSD1306_WHITE); }
    if (i == l) display.drawRect(x - 1, 38, 23, 13, SSD1306_WHITE);
    if (muteLoop[i]) display.drawLine(x, 49, x + 20, 39, has ? SSD1306_BLACK : SSD1306_WHITE);   // ปิดเสียงอยู่
    display.setCursor(x + 5, 41);
    display.print('L'); display.print(i + 1);
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(99, 41);
  display.print(dirty ? "MEL*" : "MEL");

  // แถวล่าง: สถานะ / ข้อความ | FX
  display.setCursor(0, 55);
  if (flashMsg) display.print(flashMsg);
  else          display.print(playing ? "> PLAY" : "  STOP");
  badge(101, 54, fxKeysOnly ? fxShortK[fxMode] : fxShort[fxMode], fxMode != FX_OFF);
  display.display();
}

void drawMainScreen() {
  if (melodyMode) { drawMelodyScreen(); return; }
  display.clearDisplay();
  int p = ptrn;
  int ps = playStep;

  for (int k = 0; k < NUM_STEP; k++) {
    int x = (k % 4) * 16;
    int y = (k / 4) * 16;
    bool on   = steps[p][trk][k];
    bool head = playing && (k == ps);
    if (on) display.fillRect(x + 1, y + 1, 14, 14, SSD1306_WHITE);
    else    display.fillRect(x + 6, y + 6, 4, 4, SSD1306_WHITE);
    if (head) {
      if (on) display.fillRect(x + 5, y + 5, 6, 6, SSD1306_BLACK);
      else    display.drawRect(x + 1, y + 1, 14, 14, SSD1306_WHITE);
    }
    if (playing && keyStable[k])                                     // ปุ่มที่กำลังเล่นสด
      display.fillRect(x + 3, y + 3, 10, 10, on ? SSD1306_BLACK : SSD1306_WHITE);
  }

  display.drawFastVLine(66, 0, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(71, 0);
  if (chainLen > 0) {                                   // มี chain: CH B 3/5
    display.print("CH "); display.print((char)('A' + p));
    display.print(' '); display.print((int)chainPos + 1); display.print('/'); display.print((int)chainLen);
  } else {
    display.print("PTRN "); display.print((char)('A' + p));
  }
  if (dirty) display.print(" *");
  if (usbStream) display.fillCircle(124, 3, 3, SSD1306_WHITE);   // จุด = กำลังอัดเข้าคอม

  display.setCursor(71, 11);
  display.print(playing ? instNames[liveInst] : trackNames[trk]);

  int b = bpm;
  display.setTextSize(2);
  display.setCursor(71, 23);
  display.print(b);
  display.setTextSize(1);
  display.setCursor(b >= 100 ? 109 : 97, 30);
  display.print("bpm");

  display.setCursor(71, 42);
  if (flashMsg) display.print(flashMsg);
  else          display.print(playing ? "> PLAY" : "  STOP");

  if (anyMuted()) { display.setCursor(122, 42); display.print('M'); }   // มีแทร็กปิดเสียงอยู่
  badge(71, 54, "BEAT", false);
  badge(101, 54, fxKeysOnly ? fxShortK[fxMode] : fxShort[fxMode], fxMode != FX_OFF);
  display.display();
}

void drawScreen() {
  if (mixerShown()) drawMixerScreen();
  else if (bpmHeld() && !melodyMode) drawBpmScreen();
  else           drawMainScreen();
}

// =============================================================
//  core 0 : setup / loop
// =============================================================
// ---------- คุยกับหน้าเว็บอัดเสียงผ่านสาย USB ----------
// แพ็กเก็ต: A5 5A | ชนิด | ความยาว 2 ไบต์ | ข้อมูล   (ชนิด 1 = เสียง, 2 = ข้อความสถานะ)
// คำสั่งจากคอม: '?' = ทักทาย  'R' = อัดเลย  'A' = รอกด PLAY แล้วอัด  'S' = หยุด
void usbPacket(uint8_t type, const uint8_t* data, uint16_t len) {
  uint8_t h[5] = {0xA5, 0x5A, type, (uint8_t)(len & 0xFF), (uint8_t)(len >> 8)};
  Serial.write(h, 5);
  if (len) Serial.write(data, len);
}

void usbStatus(const char* msg) {
  if (!Serial) return;
  usbPacket(2, (const uint8_t*)msg, strlen(msg));
}

void usbService() {
  bool connected = (bool)Serial;
  while (connected && Serial.available()) {
    int c = Serial.read();
    if (c == '?')      usbStatus("HELLO PocketSeq v2.2 22050");
    else if (c == 'R') { recTail = recHead; usbArmed = false; usbStream = true; }
    else if (c == 'A') { usbStream = false; recTail = recHead; usbArmed = true; usbStatus("ARM"); showMessage("USB WAIT"); }
    else if (c == 'S') { usbStream = false; usbArmed = false; }
  }
  if (!connected && (usbStream || usbArmed)) { usbStream = false; usbArmed = false; }   // ปิดหน้าเว็บ = หยุด

  static bool lastStream = false, lastPlay = false;
  if (usbStream != lastStream) {
    lastStream = usbStream;
    usbStatus(usbStream ? "REC" : "IDLE");
    showMessage(usbStream ? "USB REC" : "USB STOP");
    needRedraw = true;
  }
  if (playing != lastPlay) {
    lastPlay = playing;
    usbStatus(playing ? "PLAY" : "STOP");
  }

  // ส่งเสียงที่ค้างอยู่ในบัฟเฟอร์ ทีละไม่เกิน 512 จุด
  if (!connected) { recTail = recHead; return; }
  for (int n = 0; n < 8; n++) {
    uint32_t h = recHead, t = recTail;
    if (h == t) break;
    uint32_t cnt = (h > t) ? (h - t) : (REC_N - t);          // ส่งช่วงที่ต่อเนื่องกันก่อน
    if (cnt > 512) cnt = 512;
    usbPacket(1, (const uint8_t*)&recBuf[t], cnt * 2);
    recTail = (t + cnt) & (REC_N - 1);
  }
}

// ปลดล็อกสาย I2C: ถ้า Pico รีสตาร์ตตอนจอกำลังรับข้อมูล จอจะกดสาย SDA ค้างไว้
// ส่งสัญญาณนาฬิกา 9 ครั้งให้จอส่งข้อมูลที่ค้างจนจบ แล้วส่ง STOP ให้จอปล่อยสาย
void i2cUnstick(int sda, int scl) {
  pinMode(sda, INPUT_PULLUP);
  pinMode(scl, INPUT_PULLUP);
  delayMicroseconds(10);
  for (int i = 0; i < 9 && digitalRead(sda) == LOW; i++) {
    pinMode(scl, OUTPUT); digitalWrite(scl, LOW);  delayMicroseconds(10);
    pinMode(scl, INPUT_PULLUP);                    delayMicroseconds(10);
  }
  // STOP: SDA จากต่ำขึ้นสูง ขณะ SCL สูง
  pinMode(sda, OUTPUT); digitalWrite(sda, LOW);    delayMicroseconds(10);
  pinMode(scl, INPUT_PULLUP);                      delayMicroseconds(10);
  pinMode(sda, INPUT_PULLUP);                      delayMicroseconds(10);
}

void setup() {
  Serial.begin(115200);               // สาย USB สำหรับอัดเสียงเข้าคอม
  for (int r = 0; r < 4; r++) pinMode(rowPins[r], INPUT_PULLUP);
  for (int c = 0; c < 4; c++) pinMode(colPins[c], INPUT);
  for (int b = 0; b < NUM_BTN; b++) pinMode(btnPins[b], INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 2; i++) {
    pinMode(encClk[i], INPUT_PULLUP);
    pinMode(encDt[i],  INPUT_PULLUP);
    pinMode(encSw[i],  INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(encClk[0]), encIsrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encDt[0]),  encIsrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encClk[1]), encIsrB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encDt[1]),  encIsrB, CHANGE);

  delay(50);                          // รอจอตื่นเต็มที่
  i2cUnstick(4, 5);
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.cp437(true);                // ใช้ตารางตัวอักษรมาตรฐาน (มีรูปโน้ตดนตรี)

  EEPROM.begin(1024);

  delay(20);
  if (digitalRead(btnPins[B_SELECT]) == LOW) {
    loadDemo();
    saveNow();
    showMessage("RESET");
  } else if (loadSaved()) {
    showMessage("LOADED");
  } else {
    loadDemo();
  }

  btnRaw[B_SELECT] = btnStable[B_SELECT] = (digitalRead(btnPins[B_SELECT]) == LOW);
  selectHandled = btnStable[B_SELECT];
  wasSelectHeld = btnStable[B_SELECT];

  drawScreen();
}

void loop() {
  usbService();                       // อัดเสียงเข้าคอม (ถ้าเปิดหน้าเว็บอยู่)

  // 1) ปุ่ม 7 ตัว
  for (int b = 0; b < NUM_BTN; b++) {
    bool r = (digitalRead(btnPins[b]) == LOW);
    if (justPressed(r, btnRaw[b], btnStable[b], btnTime[b])) onButton(b);
  }

  // 2) ปุ่ม 4x4
  bool raw[16];
  readKeypad(raw);
  for (int k = 0; k < 16; k++) {
    bool before = keyStable[k];
    if (justPressed(raw[k], keyRaw[k], keyStable[k], keyTime[k])) onKey(k);
    if (before != keyStable[k] && (bpmHeld() || playing)) needRedraw = true;
    if (before && !keyStable[k]) pushEvent(EV_OFF, k, 0);    // ปล่อยปุ่ม = ปล่อยโน้ต
  }

  // 3) กดปุ่มปรับ BPM ค้าง -> ตัวเลขวิ่ง
  if (bpmHeld() && heldBpmKey >= 0) {
    if (!keyStable[heldBpmKey]) {
      heldBpmKey = -1;
    } else if ((long)(millis() - nextRepeatTime) >= 0) {
      changeBpm(bpmStepKeys[heldBpmKey]);
      nextRepeatTime = millis() + REPEAT_RATE_MS;
    }
  }
  if (wasBpmHeld != bpmHeld()) {
    if (wasBpmHeld && melodyMode && !bpmKnobUsed) melType(REST);   // โหมดเมโลดี้: ปล่อย BPM = ใส่เว้น
    wasBpmHeld = bpmHeld();
    heldBpmKey = -1;
    needRedraw = true;
  }

  // 4) ปุ่ม SELECT
  //    โหมดบีต:    ปล่อยเร็ว = ล้างแทร็กที่เลือก / กดค้าง 1 วิ = ล้างทั้งแพตเทิร์น
  //    โหมดเมโลดี้: ปล่อยเร็ว = ลบตัวล่าสุด      / กดค้าง 1 วิ = ล้างลูปที่เลือก
  bool selectHeld = btnStable[B_SELECT];
  if (selectHeld && !selectHandled && millis() - selectDownAt >= SAVE_HOLD_MS) {
    if (melodyMode) {
      clearMelodyLoop(selLoop);
      snprintf(melMsg, sizeof(melMsg), "L%d CLR", selLoop + 1);
      showMessage(melMsg);
    } else {
      for (int t = 0; t < NUM_TRK; t++) clearTrack(ptrn, t);
      showMessage("CLR ALL");
    }
    markDirty();
    selectHandled = true;
  }
  if (wasSelectHeld && !selectHeld) {
    if (!selectHandled) {
      if (melodyMode) melBackspace();
      else {
        clearTrack(ptrn, trk);
        markDirty();
        showMessage("CLEARED");
      }
    }
    selectHandled = false;
  }
  wasSelectHeld = selectHeld;

  // 4.5) ปุ่ม REC: ปล่อยเร็ว = เข้า/ออกโหมดเมโลดี้ / กดค้าง 1 วิ = เซฟ
  bool recHeld = btnStable[B_REC];
  if (recHeld && !recHandled && millis() - recDownAt >= SAVE_HOLD_MS) {
    saveNow();
    recHandled = true;
  }
  if (wasRecHeld && !recHeld) {
    if (!recHandled) {
      melodyMode = !melodyMode;
      showMessage(melodyMode ? "MELODY" : "BEAT");
    }
    recHandled = false;
  }
  wasRecHeld = recHeld;

  // 4.7) SOUND ปล่อย: ถ้าไม่ได้กดปุ่มอื่นระหว่างค้าง = ทำหน้าที่ปกติ
  bool soundHeld = btnStable[B_SOUND];
  if (wasSoundHeld != soundHeld) needRedraw = true;
  if (wasSoundHeld && !soundHeld && !soundUsed) soundAction();
  wasSoundHeld = soundHeld;
  static bool lastMixer = false;
  if (mixerShown() != lastMixer) { lastMixer = !lastMixer; needRedraw = true; }

  // 4.8) PTRN ปล่อย (โหมดบีต): ตั้ง chain / ยกเลิก chain / เปลี่ยนแพตเทิร์น
  bool ptrnHeld = btnStable[B_PTRN];
  if (wasPtrnHeld && !ptrnHeld && !melodyMode) {
    if (ptrnUsed && chainTempLen > 0) {
      for (int i = 0; i < chainTempLen; i++) chainSeq[i] = chainTemp[i];
      chainLen = chainTempLen;
      chainPos = playing ? chainLen - 1 : 0;   // กำลังเล่น: เริ่มตัวแรกตอนห้องถัดไป
      if (!playing) ptrn = chainSeq[0];
      snprintf(chainMsg, sizeof(chainMsg), "CHAIN %d", chainLen);
      showMessage(chainMsg);
    } else if (!ptrnUsed) {
      if (chainLen > 0) { chainLen = 0; showMessage("NO CHAIN"); }
      else              { ptrn = (ptrn + 1) % NUM_PTRN; markDirty(); }
    }
    needRedraw = true;
  }
  wasPtrnHeld = ptrnHeld;

  // ปิดเสียงลองฟังเครื่องดนตรี
  if (previewOffAt && (long)(millis() - previewOffAt) >= 0) {
    previewOffAt = 0;
    pushEvent(EV_OFF, 7, 0);
  }

  // 4.6) ปุ่ม FX: ปล่อยเร็ว = เปลี่ยนเอฟเฟกต์ / กดค้าง 1 วิ = สลับ FX ทุกเสียง <-> เฉพาะเล่นสด
  bool fxHeld = btnStable[B_FX];
  if (fxHeld && !fxHandled && millis() - fxDownAt >= SAVE_HOLD_MS) {
    fxKeysOnly = !fxKeysOnly;
    showMessage(fxKeysOnly ? "FX:KEYS" : "FX:ALL");
    markDirty();
    fxHandled = true;
  }
  if (wasFxHeld && !fxHeld) {
    if (!fxHandled) {
      fxMode = (fxMode + 1) % NUM_FX;
      if (fxMode != FX_OFF) lastFx = fxMode;
      showMessage(fxLong[fxMode]);
      markDirty();
    }
    fxHandled = false;
  }
  wasFxHeld = fxHeld;

  // 4.7) ปุ่มหมุน
  handleKnobs();

  // 5) ดูว่า core 1 เดินจังหวะไปสเต็ปใหม่หรือยัง -> วาดจอ + ไฟกะพริบ
  static int shownStep = -1;
  if (playing) {
    int s = playStep;
    if (s != shownStep) {
      shownStep = s;
      digitalWrite(LED_BUILTIN, s % 4 == 0);
      if (!bpmHeld()) needRedraw = true;
    }
  } else if (shownStep != -1) {
    shownStep = -1;
    needRedraw = true;
  }

  // 6) เซฟอัตโนมัติ (เฉพาะตอนหยุดเล่น)
  if (dirty && !playing && !bpmHeld() && millis() - lastChangeMs >= AUTOSAVE_DELAY_MS) {
    saveNow();
  }

  if (flashMsg && (long)(millis() - flashMsgUntil) >= 0) {
    flashMsg = nullptr;
    needRedraw = true;
  }

  // 7) วาดจอ
  if (needRedraw) {
    drawScreen();
    needRedraw = false;
  }
}

// =============================================================
//  core 1 : เสียง + นาฬิกาจังหวะ
// =============================================================
// สถานะของเสียงแต่ละตัว
float kickEnv = 0, kickPitch = 0, kickPhase = 0, kickClick = 0;

// ---- ปรับเสียงตรงนี้ได้ ----
const float KICK_VOL    = 1.2f;    // ความดัง kick  (ลองได้ 0.8 - 1.6)
const float KICK_LOW_HZ = 70.0f;   // เสียงต่ำสุดของ kick (ลำโพงเล็กเล่นต่ำกว่า ~70Hz ไม่ค่อยออก)
const float KICK_TOP_HZ = 300.0f;  // เสียงตอนเริ่มตี (ยิ่งสูง ยิ่ง "ตึ้ก" ชัด)
float snEnv = 0, snToneEnv = 0, snPhase = 0;
float hatEnv = 0, hatPrev = 0, hatDecay = 0.9985f;
float bassEnv = 0, bassPhase = 0, bassLp = 0, bassFreq = 110.0f;

inline float noteHz(int n) { return 110.0f * powf(2.0f, noteSemis[n] / 12.0f); }   // A2 = 110Hz
float crushHold = 0;
const float pitchMul = 1;         // (เดิมใช้กับ BEND ซึ่งเอาออกแล้ว)

// บัฟเฟอร์เก็บเสียงย้อนหลัง ใช้กับ DELAY (65,536 จุด ≈ 3 วินาที)
// ---- REVERB: ห้องเสียงก้องแบบ Schroeder (comb 4 ตัว + allpass 2 ตัว) ----
const int CMB_LEN[4] = {558, 594, 638, 678};
const int AP_LEN[2]  = {278, 113};
float cmbBuf[4][678];
float apBuf[2][278];
int   cmbPos[4] = {0, 0, 0, 0}, apPos[2] = {0, 0};
float cmbLp[4] = {0, 0, 0, 0};
float driveLp = 0, driveHp = 0, driveHpIn = 0;   // ฟิลเตอร์แต่งเสียงแตก

float reverbProcess(float x, float amt) {
  float fb = 0.70f + 0.26f * amt;                 // ยิ่งมาก ห้องยิ่งใหญ่ ก้องยิ่งนาน
  float y = 0;
  for (int i = 0; i < 4; i++) {
    float d = cmbBuf[i][cmbPos[i]];
    cmbLp[i] += (d - cmbLp[i]) * 0.6f;            // ลดเสียงแหลมในห้อง ให้ก้องนุ่ม
    cmbBuf[i][cmbPos[i]] = x + cmbLp[i] * fb;
    if (++cmbPos[i] >= CMB_LEN[i]) cmbPos[i] = 0;
    y += d;
  }
  y *= 0.25f;
  for (int i = 0; i < 2; i++) {
    float d = apBuf[i][apPos[i]];
    float v = y + d * 0.5f;
    apBuf[i][apPos[i]] = v;
    y = d - v * 0.5f;
    if (++apPos[i] >= AP_LEN[i]) apPos[i] = 0;
  }
  return y;
}

const uint32_t BUF_N = 65536;
const uint32_t BUF_MASK = BUF_N - 1;
int16_t  fxBuf[BUF_N];
uint32_t bufW = 0;
uint32_t stutStart = 0;
float    svfLow = 0, svfBand = 0;          // สถานะฟิลเตอร์
float    lfoFree = 0;
int   crushCnt = 0;
uint32_t rng = 22222;
bool  wasPlaying = false;

uint32_t sampleCounter = 0;

const float TWO_PI_F = 6.2831853f;

// สุ่มเลข -1..1 สำหรับเสียงซ่า (snare, hat)
inline float noise() {
  rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
  return (int32_t)rng / 2147483648.0f;
}

// ---------- เสียงเครื่องดนตรีเล่นสด (เล่นพร้อมกันได้ 4 เสียง) ----------
const float KEYS_VOL = 1.0f;          // ความดังเครื่องดนตรีเล่นสด
const int NUM_VOICES = 8;             // เมโลดี้ 4 ลูป + เล่นสด
const int MEL_KEY = 100;              // รหัสเสียงของเมโลดี้ลูป L1 = 100, L2 = 101 ... (ไม่ชนกับปุ่ม 0-15)
const int KS_MAX = 256;               // ความยาวสายสูงสุดของ PLUCK
struct Voice {
  bool on, gate;          // gate = ยังกดปุ่มค้างอยู่
  uint8_t key, inst;
  float freq, p1, p2, p3, env, env2, lp;
  uint16_t ksLen, ksPos;  // สำหรับ PLUCK
};
Voice voices[NUM_VOICES];
float ksBuf[NUM_VOICES][KS_MAX];
float sineTab[1024];

inline float sinT(float ph) {
  ph -= floorf(ph);
  return sineTab[(int)(ph * 1024) & 1023];
}

// key = รหัสเสียง (ปุ่ม 0-15 หรือ MEL_KEY), note = โน้ต 0-15
void keyNoteOn(int key, int inst, int note) {
  int vi = 0;
  float lowest = 10;
  for (int i = 0; i < NUM_VOICES; i++) {          // หาเสียงว่าง ถ้าไม่มีให้แย่งเสียงที่เบาที่สุด
    if (!voices[i].on) { vi = i; break; }
    if (voices[i].env < lowest) { lowest = voices[i].env; vi = i; }
  }
  Voice &v = voices[vi];
  v.on = true; v.gate = true; v.key = key; v.inst = inst;
  v.freq = 261.63f * powf(2.0f, keySemis[note] / 12.0f);
  v.p1 = v.p2 = v.p3 = 0; v.lp = 0; v.env2 = 1;
  v.env = isSustained(inst) ? 0 : 1;               // เครื่องที่ดังค้าง ค่อย ๆ ดังขึ้น
  if (inst == I_PLUCK) {                           // ดีดสาย: เติมสายด้วยเสียงซ่า แล้วให้มันสั่นเอง
    int len = (int)(SAMPLE_RATE / v.freq);
    if (len < 2) len = 2;
    if (len > KS_MAX) len = KS_MAX;
    v.ksLen = len; v.ksPos = 0;
    for (int j = 0; j < len; j++) ksBuf[vi][j] = noise();
  }
}

void keyNoteOff(int key) {
  for (int i = 0; i < NUM_VOICES; i++)
    if (voices[i].on && voices[i].gate && voices[i].key == key) voices[i].gate = false;
}

float renderVoices() {
  float sum = 0;
  for (int i = 0; i < NUM_VOICES; i++) {
    Voice &v = voices[i];
    if (!v.on) continue;
    float f = v.freq * pitchMul / SAMPLE_RATE;
    switch (v.inst) {
      case I_PIANO: {        // เปียโน: เสียงหลัก + ฮาร์โมนิก ดังทันทีแล้วค่อย ๆ จาง
        v.p1 += f; v.p2 += 2 * f; v.p3 += 3 * f;
        float e = v.env;
        float x = sinT(v.p1) + 0.5f * e * sinT(v.p2) + 0.25f * e * e * sinT(v.p3);
        sum += x * e * 0.45f;
        v.env *= v.gate ? 0.99994f : 0.9993f;     // ปล่อยปุ่ม = เสียงหยุดเร็วขึ้น
        break;
      }
      case I_ORGAN: {        // ออร์แกน: ดังค้างตราบที่กดปุ่ม
        v.p1 += f; v.p2 += 2 * f; v.p3 += 4 * f;
        float target = v.gate ? 1.0f : 0.0f;
        v.env += (target - v.env) * (v.gate ? 0.01f : 0.004f);
        float x = sinT(v.p1) + 0.6f * sinT(v.p2) + 0.35f * sinT(v.p3);
        sum += x * v.env * 0.28f;
        break;
      }
      case I_BELL: {         // ระฆัง: เสียงกังวาน (FM synthesis)
        v.p1 += f; v.p2 += 3.5f * f;
        float mod = sinT(v.p2) * 3.0f * v.env2;
        sum += sinT(v.p1 + mod) * v.env * 0.4f;
        v.env  *= v.gate ? 0.99997f : 0.99990f;
        v.env2 *= 0.9997f;
        break;
      }
      case I_PLUCK: {        // ดีดสาย (Karplus-Strong): จำลองสายสั่นจริง
        int len = v.ksLen;
        int a = v.ksPos, b = (a + 1) % len;
        float x = ksBuf[i][a];
        ksBuf[i][a] = 0.5f * (x + ksBuf[i][b]) * (v.gate ? 0.996f : 0.96f);
        v.ksPos = b;
        sum += x * v.env * 1.0f;
        v.env *= v.gate ? 0.99992f : 0.999f;
        break;
      }
      case I_MARIMBA: {      // มาริมบา: ไม้ตีนุ่ม เสียงสั้น
        v.p1 += f; v.p2 += 4.0f * f;
        sum += (sinT(v.p1) + 0.35f * v.env2 * sinT(v.p2)) * v.env * 0.6f;
        v.env  *= 0.99955f;
        v.env2 *= 0.9985f;
        break;
      }
      case I_CHIP: {         // เสียงเกม 8 บิต: คลื่นสี่เหลี่ยมผืนผ้า
        v.p1 += f; v.p1 -= (int)v.p1;
        float sq = (v.p1 < 0.25f) ? 1.0f : -1.0f;
        float target = v.gate ? 1.0f : 0.0f;
        v.env += (target - v.env) * (v.gate ? 0.05f : 0.01f);
        sum += sq * v.env * 0.16f;
        break;
      }
      case I_FLUTE: {        // ขลุ่ย: เสียงนุ่ม + ลม + สั่นเสียงค่อย ๆ มา
        v.p3 += 5.0f / SAMPLE_RATE;
        float vib = 1.0f + 0.006f * sinT(v.p3) * (1.0f - v.env2);
        v.p1 += f * vib; v.p2 += 2 * f * vib;
        float target = v.gate ? 1.0f : 0.0f;
        v.env += (target - v.env) * (v.gate ? 0.0025f : 0.003f);
        v.env2 *= 0.99985f;
        v.lp += (noise() * 0.12f - v.lp) * 0.2f;
        sum += (sinT(v.p1) + 0.15f * sinT(v.p2) + v.lp) * v.env * 0.45f;
        break;
      }
      case I_BRASS: {        // แตร: ฟิลเตอร์เปิด "บว้าา" ตอนเริ่มเป่า
        v.p1 += f; v.p1 -= (int)v.p1;
        float saw = v.p1 * 2 - 1;
        float target = v.gate ? 1.0f : 0.0f;
        v.env += (target - v.env) * (v.gate ? 0.004f : 0.003f);
        v.env2 *= 0.9997f;
        float cut = 0.05f + 0.45f * v.env * (0.6f + 0.4f * v.env2);
        v.lp += (saw - v.lp) * cut;
        sum += v.lp * v.env * 0.45f;
        break;
      }
      case I_LEAD: {         // ลีด: สี่เหลี่ยม + ฟันเลื่อย สดใส มีสั่นเสียง
        v.p3 += 5.5f / SAMPLE_RATE;
        float vib = 1.0f + 0.004f * sinT(v.p3);
        v.p1 += f * vib;          v.p1 -= (int)v.p1;
        v.p2 += f * vib * 1.01f;  v.p2 -= (int)v.p2;
        float x = ((v.p1 < 0.5f) ? 0.5f : -0.5f) + (v.p2 - 0.5f);
        float target = v.gate ? 1.0f : 0.0f;
        v.env += (target - v.env) * (v.gate ? 0.03f : 0.006f);
        v.lp += (x - v.lp) * 0.45f;
        sum += v.lp * v.env * 0.3f;
        break;
      }
      case I_SYNTH: {        // ซินธ์: คลื่นฟันเลื่อย 2 ตัวเพี้ยนกันนิด ๆ ให้เสียงอ้วน
        v.p1 += f; v.p2 += f * 1.007f;
        v.p1 -= (int)v.p1;  v.p2 -= (int)v.p2;
        float saw = (v.p1 * 2 - 1 + v.p2 * 2 - 1) * 0.5f;
        float target = v.gate ? 1.0f : 0.0f;
        v.env += (target - v.env) * (v.gate ? 0.02f : 0.003f);
        v.lp += (saw - v.lp) * (0.12f + 0.25f * v.env);
        sum += v.lp * v.env * 0.35f;
        break;
      }
    }
    if (v.p1 > 1000) { v.p1 -= (int)v.p1; v.p2 -= (int)v.p2; v.p3 -= (int)v.p3; }
    bool sustained = v.gate && isSustained(v.inst);
    if (v.env < 0.0005f && !sustained) v.on = false;
  }
  return sum;
}

void triggerTrack(int t) {
  switch (t) {
    case 0: kickEnv = 1; kickPitch = 1; kickPhase = 0; kickClick = 1; break;   // KICK
    case 1: snEnv = 1; snToneEnv = 1;                  break;   // SNARE
    case 2: hatEnv = 1; hatDecay = 0.9985f;            break;   // HAT
    case 3: bassEnv = 1;                               break;   // BASS
  }
}

// =============================================================
//  เมโลดี้: เล่น 4 ลูปพร้อมกัน (core 1)
// =============================================================
void melodyStep(int s) {
  for (int l = 0; l < MEL_LOOPS; l++) {
    uint8_t n = melNote[l][s];
    if (n < 16 && !muteLoop[l]) {                       // โน้ต: ตัดตัวเก่าของลูปนี้ แล้วเล่นตัวใหม่
      keyNoteOff(MEL_KEY + l);
      keyNoteOn(MEL_KEY + l, melInst[l], n);
    } else {                            // เว้น / ยังว่าง = เงียบ
      keyNoteOff(MEL_KEY + l);
    }
  }
}

bool firstBar = false;

void melodyAllOff() {
  for (int l = 0; l < MEL_LOOPS; l++) keyNoteOff(MEL_KEY + l);
}

void setup1() {
  for (int i = 0; i < 1024; i++) sineTab[i] = sinf(i * TWO_PI_F / 1024);
  i2s.setBCLK(I2S_BCLK);
  i2s.setDATA(I2S_DIN);
  i2s.setBitsPerSample(16);
  i2s.begin(SAMPLE_RATE);
}

// loop1 ทำงานครั้งละ 1 จุดเสียง  (22,050 ครั้งต่อวินาที)
void loop1() {
  // เสียงที่ core 0 ขอให้เล่นทันที (ตอนกดลองฟัง / อัดสด)
  for (int t = 0; t < NUM_TRK; t++) {
    if (trig[t]) { trig[t] = false; triggerTrack(t); }
  }
  if (trigOpenHat) { trigOpenHat = false; hatEnv = 1; hatDecay = 0.99975f; }   // ไฮแฮตเปิด เสียงยาวกว่า
  int tn = trigNote;
  if (tn >= 0) { trigNote = -1; bassFreq = noteHz(tn); bassEnv = 1; }
  while (evTail != evHead) {                     // ข้อความกด/ปล่อยโน้ตจาก core 0
    uint32_t e = evq[evTail];
    evTail = (evTail + 1) % EVQ_N;
    uint8_t type = e & 0xFF, key = (e >> 8) & 0xFF, inst = (e >> 16) & 0xFF;
    if (type == EV_ON) keyNoteOn(key, inst, key);
    else               keyNoteOff(key);
  }

  // นาฬิกาจังหวะ: นับจำนวนจุดเสียงที่ส่งออก จึงแม่นยำมาก
  if (playing) {
    uint32_t base = (uint32_t)SAMPLE_RATE * 15 / bpm;   // 1/16 โน้ต
    // SWING: ช่องคี่ (จังหวะยก) ถูกเลื่อนให้ช้าลง -> ช่องคู่ยาวขึ้น ช่องคี่สั้นลง
    float sw = swingPct * 0.0033f;                      // 100% = เลื่อน 1/3 ช่อง (แบบสามพยางค์)
    uint32_t stepLen = (playStep % 2 == 0) ? (uint32_t)(base * (1.0f + sw)) : (uint32_t)(base * (1.0f - sw));
    if (!wasPlaying) {
      wasPlaying = true;
      if (usbArmed) { usbArmed = false; usbStream = true; }   // เริ่มอัดพร้อมจังหวะแรกพอดี
      playStep = NUM_STEP - 1;
      sampleCounter = 0xFFFFFF;              // ให้เริ่มสเต็ป 1 ทันที
      firstBar = true;
      if (chainLen > 0) { chainPos = 0; ptrn = chainSeq[0]; }
    }
    if (++sampleCounter >= stepLen) {
      sampleCounter = 0;
      int s = (playStep + 1) % NUM_STEP;
      playStep = s;
      if (s == 0) {                          // ต้นห้อง: chain ไปแพตเทิร์นถัดไป
        if (!firstBar && chainLen > 0) {
          chainPos = (chainPos + 1) % chainLen;
          ptrn = chainSeq[chainPos];
        }
        firstBar = false;
      }
      if (s % 4 == 0) stutStart = bufW;
      int p = ptrn;
      for (int t = 0; t < NUM_TRK; t++) {
        if (steps[p][t][s] && !muteTrk[t]) {
          if (t == 3) bassFreq = noteHz(bassNote[p][s]);
          triggerTrack(t);
        }
      }
      melodyStep(s);
    }
  } else if (wasPlaying) {
    wasPlaying = false;
    melodyAllOff();
  }

  float out = 0;


  // KICK: คลื่นไซน์ที่เสียงตกจากสูงลงต่ำเร็ว ๆ = "ตุ้บ"
  //  v0.7: ยกเสียงต่ำสุดขึ้นให้ลำโพงเล็กเล่นได้ + ทำให้เสียงอิ่มขึ้น (saturation)
  //        + เพิ่มเสียง "ตึ้ก" สั้น ๆ ตอนเริ่มตี ช่วยให้หูจับจังหวะได้
  if (kickEnv > 0.0005f) {
    float f = KICK_LOW_HZ + (KICK_TOP_HZ - KICK_LOW_HZ) * kickPitch;
    kickPhase += f * pitchMul / SAMPLE_RATE;
    if (kickPhase >= 1) kickPhase -= 1;
    float k = sinf(kickPhase * TWO_PI_F) * 2.5f;
    k = k / (1.0f + fabsf(k));                 // บีบให้อิ่ม เกิดเสียงสูงเพิ่ม ลำโพงเล็กได้ยินชัดขึ้น
    out += k * kickEnv * KICK_VOL;
    if (kickClick > 0.001f) {                  // เสียงตึ้กตอนเริ่ม ยาวไม่กี่มิลลิวินาที
      out += noise() * kickClick * 0.35f;
      kickClick *= 0.985f;
    }
    kickEnv   *= 0.99970f;
    kickPitch *= 0.9985f;
  }

  // SNARE: เสียงซ่า + เสียงโทนสั้น ๆ = "แต๊ก"
  if (snEnv > 0.0005f) {
    snPhase += 190.0f * pitchMul / SAMPLE_RATE;
    if (snPhase >= 1) snPhase -= 1;
    out += noise() * snEnv * 0.35f + sinf(snPhase * TWO_PI_F) * snToneEnv * 0.3f;
    snEnv     *= 0.99960f;
    snToneEnv *= 0.9990f;
  }

  // HAT: เสียงซ่าเฉพาะย่านแหลม สั้นมาก = "ชิ"
  if (hatEnv > 0.0005f) {
    float n = noise();
    out += (n - hatPrev) * hatEnv * 0.18f;
    hatPrev = n;
    hatEnv *= hatDecay;
  }

  // BASS: คลื่นฟันเลื่อย ผ่านฟิลเตอร์ที่ค่อย ๆ ปิด = "บึ้ม"
  if (bassEnv > 0.0005f) {
    bassPhase += bassFreq * pitchMul / SAMPLE_RATE;
    if (bassPhase >= 1) bassPhase -= 1;
    float saw = bassPhase * 2 - 1;
    float c = (0.02f + 0.25f * bassEnv) * sqrtf(bassFreq / 110.0f);   // โน้ตสูง ฟิลเตอร์เปิดกว้างตาม
    if (c > 0.95f) c = 0.95f;
    bassLp += (saw - bassLp) * c;
    out += bassLp * bassEnv * 0.6f;
    bassEnv *= 0.99982f;
  }

  // แยกเสียงบีตกับเครื่องดนตรีเล่นสด เพื่อเลือกว่าอะไรผ่าน FX
  float keys = renderVoices() * KEYS_VOL;
  float bypass = 0;                                // เสียงที่ไม่ผ่าน FX
  if (fxKeysOnly) { bypass = out; out = keys; }    // FX:KEYS -> บีตไม่ผ่าน FX
  else           { out += keys; }                 // FX:ALL  -> ทุกเสียงผ่าน FX

  // =========================================================
  //  FX
  // =========================================================
  float dry = out;
  float store = dry;                                  // ค่าที่จะเก็บลงบัฟเฟอร์
  uint32_t sps = (uint32_t)SAMPLE_RATE * 15 / bpm;    // จำนวนจุดเสียงต่อ 1 สเต็ป
  int m = fxMode;

  float amt = fxAmount;
  if (m == FX_OD) {
    // OVERDRIVE: แตกนุ่ม อุ่น แบบแอมป์หลอดที่เร่งเสียง
    float g = 1.5f + 6.0f * amt;
    float x = dry * g;
    x = x / (1.0f + fabsf(x));                         // บีบนุ่ม ๆ
    driveLp += (x - driveLp) * 0.35f;                 // ตัดแหลมให้เสียงอุ่น
    out = driveLp * 0.55f;

  } else if (m == FX_DIST) {
    // DISTORTION: ตัดเบสก่อนให้เสียงแน่น แล้วอัดแตกหนักแบบร็อก
    driveHp = 0.97f * (driveHp + dry - driveHpIn);    // ตัดเสียงต่ำ
    driveHpIn = dry;
    float g = 6.0f + 40.0f * amt;
    float x = driveHp * g;
    if (x >  1) x =  1;
    if (x < -1) x = -1;
    x = x * 1.5f - 0.5f * x * x * x;                   // ขอบมน ไม่แข็งเกินไป
    driveLp += (x - driveLp) * 0.3f;
    out = driveLp * 0.3f;

  } else if (m == FX_FUZZ) {
    // FUZZ: แตกโบราณยุค 60s แตกไม่เท่ากันบน-ล่าง เสียงหนาเกือบเป็นคลื่นสี่เหลี่ยม
    float g = 20.0f + 80.0f * amt;
    float x = dry * g + 0.15f;                         // เอียงนิดหน่อย = แตกไม่สมมาตรแบบทรานซิสเตอร์เก่า
    if (x >  0.6f) x =  0.6f;
    if (x < -1.0f) x = -1.0f;
    driveLp += (x - 0.15f * 0.6f - driveLp) * 0.18f;   // อู้ ๆ หนา ๆ
    out = driveLp * 0.35f;

  } else if (m == FX_REVERB) {
    // REVERB: ก้องแบบห้องโถง
    out = dry + reverbProcess(dry, amt) * (0.35f + 0.25f * amt);

  } else if (m == FX_DELAY) {
    // DELAY: เสียงสะท้อนห่าง 3 สเต็ป วนซ้ำค่อย ๆ เบาลง
    uint32_t ds = sps * 3;
    if (ds > BUF_N - 1) ds = BUF_N - 1;
    float d = fxBuf[(bufW - ds) & BUF_MASK] / 32767.0f;
    out   = dry + d * (0.25f + 0.6f * amt);           // ยิ่งมาก เสียงสะท้อนยิ่งดังและยาว
    store = dry + d * (0.15f + 0.6f * amt);
  }

  // เก็บเสียงลงบัฟเฟอร์ทุกจุด (เพื่อให้ ECHO มีเสียงย้อนหลังใช้ทันที)
  if (store >  1) store =  1;
  if (store < -1) store = -1;
  fxBuf[bufW] = (int16_t)(store * 32767);
  bufW = (bufW + 1) & BUF_MASK;

  out += bypass;                                  // ใส่บีตกลับเข้ามา (โหมด FX:KEYS)

  if (usbStream) {                     // ส่งเสียงเข้าคอม (ไม่ขึ้นกับปุ่มความดัง ไฟล์จึงดังเท่าเดิมเสมอ)
    float r = out * 0.7f;              // เท่ากับความดังเริ่มต้น เผื่อที่ไว้ไม่ให้เสียงแตก
    if (r >  1) r =  1;
    if (r < -1) r = -1;
    uint32_t h = recHead, nh = (h + 1) & (REC_N - 1);
    if (nh != recTail) { recBuf[h] = (int16_t)(r * 30000); recHead = nh; }
  }

  out *= masterVol;                   // ระดับเสียงรวม
  if (out >  1) out =  1;
  if (out < -1) out = -1;
  int16_t s16 = (int16_t)(out * 30000);
  i2s.write16(s16, s16);               // ส่งเสียงออกซ้าย-ขวา (รอจนกว่าแอมป์พร้อมรับ)
}
