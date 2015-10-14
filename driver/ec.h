
UCHAR ECWaitRead () {
USHORT Timeout = 0x7FF;
while (Timeout>0) {
if ((INB(0x66) & 0x01) == 0x01) break; 
KeStallExecutionProcessor(10);
Timeout--;
}
if (((INB(0x66) & 0x01) == 0x00) && (Timeout == 0)) return 0;
return 1;
}

UCHAR ECWaitWrite () {
USHORT Timeout = 0x7FF;
while (Timeout>0) {
if ((INB(0x66) & 0x02) == 0x00) break; 
KeStallExecutionProcessor(10);
Timeout--;
}
if (((INB(0x66) & 0x02) == 0x02) && (Timeout == 0)) return 0;
return 1;
}

UCHAR ReceiveECData (IN PBYTE Data) {
if (ECWaitRead() != 1) {
return 0;
}
* Data = INB(0x62);
return 1;
}

UCHAR SendECCommand (IN UCHAR Com) {
if (ECWaitWrite() != 1) return 0;
OUTB(0x66, Com);
return 1;
}

UCHAR SendECData (IN UCHAR Data) {
if (ECWaitWrite() != 1) return 0;
OUTB(0x62, Data);
if (ECWaitWrite() != 1) return 0;
return 1;
}

UCHAR ECReadByteHelper(IN UCHAR Offs, IN PBYTE Data) {
BYTE Retries = 10;
while (Retries > 0) { 
if (SendECCommand(0x80) != 1) {
Retries--;
continue;
}
if (SendECData(Offs) != 1) {
Retries--;
continue;
}
if (ReceiveECData(Data) != 1) {
Retries--;
continue;
}
break;
}
//DbgPrint("Retries remained: %d", Retries);
if (Retries > 0) return 1; else return 0;
}

UCHAR ECReadByte(IN UCHAR Offs, IN PBYTE Data) {
/*if (SendECCommand(0x80) != 1) {
*DbgPrint("Timeout at sending command");
*return 0;}
*if (SendECData(Offs) != 1) {
*DbgPrint("Timeout at sending offset");
*return 0;}
*if (ReceiveECData(Data) != 1) {
*DbgPrint("Timeout at receiving data");
*return 0;
*}
*return 1;*/
if (ECReadByteHelper(Offs, Data) != 1) return 0;
return 1;
}

UCHAR ECWriteByte(IN UCHAR Offs, IN UCHAR Data) {
if (SendECCommand(0x81) != 1) return 0;
if (SendECData(Offs) != 1) return 0;
if (SendECData(Data) != 1) return 0;
return 1;
}

UCHAR ECEnterBurst () {
UCHAR Data = 0;
if (SendECCommand(0x82) != 1) return 0;
if (ReceiveECData(&Data) != 1) return 0;
if (Data != 0x90) return 0;
return 1;
}

UCHAR ECExitBurst () {
if (SendECCommand(0x83) != 1) return 0;
return 1;
}

UCHAR ECBurstRead (IN UCHAR Offs, IN UCHAR Len, IN PBYTE Buffer) {
ULONG i = 0;
if (ECEnterBurst() != 1) return 0;
for (i = (ULONG)(Offs); i < (ULONG)((ULONG)(Offs)+(ULONG)(Len)); i++) {if (ECReadByte((UCHAR)(i), &Buffer[(ULONG)(i-(ULONG)(Offs))]) != 1) return 0;}
ECExitBurst();
return 1;
}

UCHAR ECBurstWrite (IN UCHAR Offs, IN UCHAR Len, IN PBYTE Buffer) {
ULONG i = 0;
if (ECEnterBurst() != 1) return 0;
for (i = (ULONG)(Offs); i < (ULONG)((ULONG)(Offs)+(ULONG)(Len)); i++) {if (ECWriteByte((UCHAR)(i), Buffer[(ULONG)(i-(ULONG)(Offs))]) != 1) return 0;}
ECExitBurst();
return 1;
}
