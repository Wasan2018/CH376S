#include <SoftwareSerial.h>
#define rx 10
#define tx 11
byte CMD_GET_IV_VER[1] = {0x01};
byte CMD_SET_BAUDRATE[1] = {0x02};
byte CMD_ENTER_SLEEP[1] = {0x03};
byte CMD_RESET_ALL[1] = {0x05};
byte CMD_CHECK_EXIST[2] = {0x06, 0x01};
byte CMD_SET_SD0_INT[1] = {0x0B};      //-----ยังไม่สมบูรณ์----------//
byte CMD_FILE_SIZE[2] = {0x0C, 0x68};  //----ยังไม่แน่ใจ-----//
byte CMD_SET_USB_MODE[2] = {0x15, 0x06};
byte CMD_GET_STATUS[1] = {0x22};
byte CMD_RD_USB_DATA0[1] = {0x27};
byte CMD_WR_USB_DATA[] = {}; // DATA LENGTH // DATA STREAM(N)
byte CMD_WR_REQ_DATA[] = {}; //             // DATA STREAM(N)
byte CMD_WR_OFS_DATA[1] = {0x2E}; // DATA LENGTH // DATA STREAM
byte CMD_SET_FILE_NAME[2] = {0x2f, 0x2f}; // following with CHARECTOR STRING(N)
byte CMD_DISK_CONNECT[1] = {0x30};
byte CMD_DISK_MOUNT[1] = {0x31};

byte CMD_FILE_OPEN[1] = {0x32};
byte CMD_FILE_ENUM_GO[1] = {0x33};
byte CMD_FILE_CREATE[3] = {0x34, 0x36, 0x38};
byte CMD_FILE_ERASE[1] = {0x35};
byte CMD_FILE_CLOSE[2] = {0x36, 0x01};

byte CMD_INFO_READ[1] = {0x37}; // Following catalog index number
byte CMD_INFO_SAVE[1] = {0x38};

byte CMD_BYTE_LOCATE[1] = {0x39};  //EXCURSION BYTE NUMBER(4)
byte CMD_BYTE_READ[1] = {0x3A};     //REQUEST BYTE NUMBER(2)
byte CMD_BYTE_RD_GO[1] = {0x3B};
byte CMD_BYTE_WRITE[1] = {0x3C};     //REQUEST BYTE NUMBER(2)
byte CMD_BYTE_WR_GO[1] = {0x3D};

byte CMD_DISK_CAPACITY[1] = {0x3E};
byte CMD_DISK_QUERY[1] = {0x3F};
byte CMD_DIR_CREATE[1] = {0x40};

byte CMD_SEG_LOCATE[1] = {0x4A};  //EXCURSION FAN NUMBER(4)
byte CMD_SEG_READ[1] = {0x4B};  //REQUEST FAN NUMBER
byte CMD_SEG_WRITE[1] = {0x4C}; //REQUEST FAN NUMBER

byte CMD_DISK_BOC_CMD[1] = {0x50};
byte CMD_DISK_READ[1] = {0x54}; //LBA FAN ADDRESS / SECTOR NUMBER
byte CMD_DISK_RD_GO[1] = {0x55};
byte CMD_DISK_WRITE[1] = {0x56}; //LBA ADDRESS / SECTOR NUMBER
byte CMD_DISK_WR_GO[1] = {0x57};

byte CMD_SET_SDCARD_MODE[2] = {0x15, 0x03};
byte CMD_CHECK_SERAIL_CONNECTIONS[2] = {0x06, 0x57};

int timeOut = 2000;
boolean SD_Card_Mode = false;
String fileName = "";
String FileTemp = "abc";
String listTemp[10] = {};
SoftwareSerial CH376S(rx, tx);
void setup() {
  CH376S.begin(9600);
  Serial.begin(9600);
  Serial.println("Let's Start");
  pinMode(13, OUTPUT);
}
void loop() {
  if (Serial.available()) {
    byte GET_CMD = Serial.read();
    Serial.print("Sending:");
    Serial.println(GET_CMD);
    if (GET_CMD == 48) { //0
      SendCommand(CMD_RESET_ALL, sizeof(CMD_RESET_ALL), false);
    }
    if (GET_CMD == 49) { //1
      SendCommand(CMD_CHECK_SERAIL_CONNECTIONS, sizeof(CMD_CHECK_SERAIL_CONNECTIONS), false);
      if (waitForResponse("Check connection"))
        Serial.println("STATUS OK");
    }
    if (GET_CMD == 50) { // 2
      SendCommand(CMD_SET_SDCARD_MODE, sizeof(CMD_SET_SDCARD_MODE), false);
    }
    if (GET_CMD == 51) { //3
      //  SendCommand(CMD_SET_SDCARD_MODE, false);
      CreateFile("Text.txt");
      //SetConnectionMode(2);
    }
    if (GET_CMD == 52) { // 4
      FileList();
    }
    if (GET_CMD == 53) { // 5
      SendCommand(CMD_SET_USB_MODE, sizeof(CMD_SET_USB_MODE), false);
    }
    if (GET_CMD == 54) { // 6
      DeleteFile("Text.txt");
    }
    if (GET_CMD == 55) { //7

      Serial.print("File check status : ");
      Serial.println(CanOpen("Text.txt"));
    }
    if (GET_CMD == 56) { //8
      FileList();
    }
    //enumGo
    if (GET_CMD == 97) { //a
      nextList();
    }
    if (GET_CMD == 57) { //9
      Serial.println(getResFileName(FileTemp));
    }
    if (GET_CMD == 99) { //c
      Serial.println("All files");
      int total = 0;
      for (int i = 0; listTemp[i] != NULL; i++) {
        Serial.println(listTemp[i]);
        total++;
        if (i > sizeof(listTemp) / sizeof(String))
          break;
      }
      Serial.print("Total:");
      Serial.println(total);
    }
    if (GET_CMD == 98) { // b

      int count = 0;
      //  nextList();
      while (1) {
        String FileFormSD = getResFileName(FileTemp);
        if (count > 1 && FileFormSD == "")
          break;
        nextList();
        delay(1);
        CH376S_Response();
        // delay(100);
        if (count != 0) {
          // listTemp[count] = FileFormSD;
          Serial.println(".");
          listTemp[count - 1] = FileFormSD;
        }
        count++;
      }

      Serial.println("Exit List");
    }

  }
  CH376S_Response();
  // delay(50);
}
void bliyn() {
  digitalWrite(13, 1);
  delay(100);
  digitalWrite(13, 0);
  delay(100);
}
void CH376S_Response() {
  String TempStr = "";
  boolean check = false;
  if (CH376S.available() > 0) {
    byte data = CH376S.read();
    // byte stringOne =  String(45, HEX);
    //    Serial.println(stringOne);
    /*Serial.print("Response code:");
      Serial.println(data, HEX);
      Serial.print("Response to text: ");*/
    FileTemp =  CH376S.readString();
  }

}
/*
  SendCommand(A,B)
  พารามิเตอร์ A เป็นคำสั่งหรือ CMD ที่จะทำ
  พารามิเตอร์ B เป็น Null ถ้า true = byte สุดท้ายที่ส่งจะมีค่าเป็น NULL
                       ถ้า false = byte สุดท้ายที่ส่งจะมีค่าเป็นเป็นปกติ
*/
String getResFileName(String str) {
  String temp = "";
  //Serial.println(   );
  if (str.indexOf("TXT") == 9) { //is text file
    for (int i = 1; i < 8; i++) {
      if (i == 7 && str[i] != ' ')
        temp[7] = ' ';
      temp += str[i];
      if (i > 7)
        break;
    }
  }
  return temp;
}
void SendCommand( byte CMD[], int Size, boolean nullByte) {
  CH376S.write(0x57);
  CH376S.write(0xAB);
  for (int loop_ = 0; loop_ < Size / sizeof(byte); loop_++) {
    CH376S.write(CMD[loop_]);
  }
  if (nullByte)
    CH376S.write((byte)0x00);
}
void SetConnectionMode(byte mode) {
  if (mode <= 0 || mode >= 3) //mode 1 is SD card  /mode 2 is USB
    return ;
  if (mode == 1) {
    SD_Card_Mode = true;
    SendCommand(CMD_SET_SDCARD_MODE, sizeof(CMD_SET_SDCARD_MODE), false);
  }
  else {
    SD_Card_Mode = false;
    SendCommand(CMD_SET_USB_MODE, sizeof(CMD_SET_USB_MODE), false);
  }
}
byte getMode() {
  return SD_Card_Mode;
}
boolean waitForResponse(String errorMsg) {
  boolean bytesAvailable = true;
  int counter = 0;
  while (!CH376S.available()) {
    delay(1);
    counter++;
    if (counter > timeOut) {
      Serial.print("TimeOut waiting for response: Error while: ");
      Serial.println(errorMsg);
      bytesAvailable = false;
      break;
    }
  }
  delay(1);
  return (bytesAvailable);
}
void FileList() {
  SetFilename("*");
  CMD_SendFileName();
  SendCommand(CMD_FILE_OPEN, sizeof(CMD_FILE_OPEN), false);
  enumGo();
  SendCommand(CMD_RD_USB_DATA0, sizeof(CMD_RD_USB_DATA0), false);

}
void nextList() {
  // enumGo();
  SendCommand(CMD_FILE_ENUM_GO, sizeof(CMD_FILE_ENUM_GO), false);
  SendCommand(CMD_RD_USB_DATA0, sizeof(CMD_RD_USB_DATA0), false);
}
void enumGo() {
  SendCommand(CMD_FILE_ENUM_GO, sizeof(CMD_FILE_ENUM_GO), false);
}
String SerialRecTraslator(byte CMD) {
  String Temp = "";
  switch (CMD) {
    case 0x14:
      Temp = "Success of SD card or USB transaction or tranfer operation";
      break;
    case 0x15:
      Temp = "Detection of USB device attachment";
      break;
    case 0x16:
      Temp = "Detection of USB device attachment";
      break;
    case 0x17:
      Temp = "Data error or Buffer overflow";
      break;
    case 0x18:
      Temp = "USB device has initialized";
      break;
    case 0x1d:
      Temp = "Read operation of storage device";
      break;
    case 0x1e:
      Temp = "Write operation of storage device";
      break;
    case 0x1f:
      Temp = "Failure of USB storage device";
      break;
    ////////////////////SD or USB Mode//////////////////
    case 0x41:
      Temp = "ERR_OPEN_DIR";
      break;
    case 0x42:
      Temp = "ERR_MISS_FILE";
      break;
    case 0x43:
      Temp = "ERR_FOUND_NAME";
      break;
    case 0x82:
      Temp = "ERR_DISK_DISCON";
      break;
    case 0x84:
      Temp = "ERR_LARGE_SECTOR";
      break;
    case 0x92:
      Temp = "ERR_TYPE_ERROR";
      break;
    case 0x1a:
      Temp = "ERR_BPB_ERROR";
      break;
    case 0xb1:
      Temp = "ERR_DISK_FULL";
      break;
    case 0xb2:
      Temp = "ERR_FDT_OVER";
      break;
    case 0xb4:
      Temp = "ERR_FILE_CLOSE";
      break;
    case 0x51:
      Temp = "Operation successful";
      break;
    case 0x5f:
      Temp = "Operation failure";
      break;
    case 0xa8:
      Temp = "Commu Connected";
      break;
    default:
      Temp = "Unknow";
      break;
  }
  return Temp;
}
void ResetAll() {
  SendCommand(CMD_RESET_ALL, sizeof(CMD_RESET_ALL), false);
}
void DiskMount() {
  SendCommand(CMD_DISK_MOUNT, sizeof(CMD_FILE_CREATE), false);
  if (waitForResponse("mounting USB disk")) {     //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
    if (getMode() == 1) {
      Serial.println(">SD-Card Mounted - OK");
    } else {
      if (getResponseFromUSB() == 0x14) {           //CH376S will send 0x14 if this command was successful
        Serial.println(">USB Mounted - OK");
      } else {
        Serial.print(">Failed to Mount USB disk.");
      }
    }
  }
}
void CreateFile(String fileName) {
  SetFilename(fileName);
  delay(50);
  CMD_SendFileName();
  delay(50);
  SendCommand(CMD_FILE_CREATE, sizeof(CMD_FILE_CREATE), false);
  delay(50);
  //SendCommand(CMD_FILE_CLOSE, false);
  delay(50);
  Serial.println("File Created");
}
void SetFilename(String name_) {
  fileName = name_;
}
void CMD_SendFileName() {
  SendCommand(CMD_SET_FILE_NAME, sizeof(CMD_SET_FILE_NAME), false); //------------
  CH376S.print(GetFileName());
  //Serial.print(GetFileName());
  CH376S.write((byte)0x00);
  // Serial.println((byte)0x00);
}
String GetFileName() {
  return fileName;
}
void DeleteFile(String textName) {
  SetFilename(textName);
  CMD_SendFileName();
  SendCommand(CMD_FILE_ERASE, sizeof(CMD_FILE_ERASE), false);
  Serial.print("Delete File name: ");
  Serial.println(textName);
}
void DiskMountConnectionStatus() {
  SendCommand(CMD_DISK_CONNECT, sizeof(CMD_DISK_CONNECT), false);
}
byte getResponseFromUSB() {
  byte response = byte(0x00);
  if (CH376S.available()) {
    response = CH376S.read();
  }
  return (response);
}
boolean CanOpen(String textName) {
  boolean temp = false;
  SetFilename(textName);
  CMD_SendFileName();
  SendCommand(CMD_FILE_OPEN, sizeof(CMD_FILE_OPEN), false);
  //Serial.println("OpenFile");
  if (waitForResponse("get file can open")) {
    if (getResponseFromUSB() == 0x14)
      temp  = true;
  }
  SendCommand(CMD_FILE_CLOSE, sizeof(CMD_FILE_CLOSE), false);
  return temp;
}
void CreateFolder(String Name) {
  SetFilename(Name);
  CMD_SendFileName();
  SendCommand(CMD_DIR_CREATE, sizeof(CMD_DIR_CREATE), false);
  Serial.println("Create Folder");
}

