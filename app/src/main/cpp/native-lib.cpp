#include <jni.h>
#include "protocol.h"




extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_startProtocol(
        JNIEnv *env,
        jobject /* this */) {
    vendingMachineProtocol.startProtocol();
}

extern "C"

jint
Java_vmc_machine_impl_kubota_KubotaImpl_startProtocol1(
        JNIEnv *env,
        jobject /* this */, jbyteArray portNameIn, jint baudRateIn, jboolean isDebug) {
    jbyte *portNameAddr = env->GetByteArrayElements(portNameIn, false);
    return vendingMachineProtocol.startProtocol((char*)portNameAddr, baudRateIn, isDebug);
}

extern "C"

jint
Java_vmc_machine_impl_kubota_KubotaImpl_getEvent(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getEvent();
}

extern "C"

jshort
Java_vmc_machine_impl_kubota_KubotaImpl_getSelectedGoodAmount(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getSelectedGoodAmount();
}

extern "C"

jbyte
Java_vmc_machine_impl_kubota_KubotaImpl_getSelectedGoodColumnNo(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getSelectedGoodColumnNo();
}

extern "C"

jbyte
Java_vmc_machine_impl_kubota_KubotaImpl_getCurrentSalesColumnNo(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getCurrentSalesColumnNo();
}

extern "C"

jbyteArray
Java_vmc_machine_impl_kubota_KubotaImpl_getCurrentSalesCount(
        JNIEnv *env,
        jobject  /* this */) {
    jbyteArray ret = (*env).NewByteArray(4);
    jbyte salesCount[4];

    vendingMachineProtocol.getCurrentSalesCount((unsigned char*)salesCount);
    (*env).SetByteArrayRegion(ret, 0, 4, salesCount);
    return ret;
}

extern "C"

jbyteArray
Java_vmc_machine_impl_kubota_KubotaImpl_getCurrentSalesAmount(
        JNIEnv *env,
        jobject  /* this */) {
    jbyteArray ret = (*env).NewByteArray(4);
    jbyte salesAmount[4];

    vendingMachineProtocol.getCurrentSalesAmount((unsigned char*)salesAmount);
    (*env).SetByteArrayRegion(ret, 0, 4, salesAmount);
    return ret;
}

extern "C"

jboolean
Java_vmc_machine_impl_kubota_KubotaImpl_getSetPriceResults(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getSetPriceResults();
}

extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_initialization(
        JNIEnv *env,
        jobject  /* this */) {
    vendingMachineProtocol.initialization();
}

extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_outGoods(
        JNIEnv *env,
        jobject  /* this */) {
    vendingMachineProtocol.outGoods();
}

extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_outGoodsUseB0ByButton(
        JNIEnv *env,
        jobject  /* this */, jbyte salesCategory) {
    vendingMachineProtocol.outGoodsUseB0ByButton(salesCategory);
}

extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_outGoodsUseB0(
        JNIEnv *env,
        jobject  /* this */, jbyte columnNo, jbyte salesCategory) {
    vendingMachineProtocol.outGoodsUseB0(columnNo, salesCategory);
}

extern "C"

jint
Java_vmc_machine_impl_kubota_KubotaImpl_getColumnCount(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getColumnCount();
}

extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_setColumnPrice(
        JNIEnv *env,
        jobject  /* this */, jbyteArray price) {
    jbyte *priceAddr = env->GetByteArrayElements(price, false);
    vendingMachineProtocol.setColumnPrice((unsigned char*)priceAddr);
}

extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_cancelTransaction(
        JNIEnv *env,
        jobject  /* this */) {
    vendingMachineProtocol.cancelTransaction();
}

extern "C"

void
Java_vmc_machine_impl_kubota_KubotaImpl_selectGoodsByScreen(
        JNIEnv *env,
        jobject  /* this */, jbyte columnNo) {
    vendingMachineProtocol.selectGoodsByScreen(columnNo);
}

extern "C"

jbyteArray
Java_vmc_machine_impl_kubota_KubotaImpl_getGoodsSoldOutState(
        JNIEnv *env,
        jobject  /* this */) {
    int i = vendingMachineProtocol.getColumnCount();
    jbyteArray ret = (*env).NewByteArray((i + 7) / 8);
    jbyte goodsSoldOutState[(200 + 7) / 8];

    vendingMachineProtocol.getGoodsSoldOutState((unsigned char*)goodsSoldOutState);
    (*env).SetByteArrayRegion(ret, 0, (i + 7) / 8, goodsSoldOutState);
    return ret;
}

extern "C"

jbyte
Java_vmc_machine_impl_kubota_KubotaImpl_getStateData(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getStateData();
}

extern "C"

jbyteArray
Java_vmc_machine_impl_kubota_KubotaImpl_getFaultInformation(
        JNIEnv *env,
        jobject  /* this */) {
    int i = vendingMachineProtocol.getFaultInformationCount();
    jbyteArray ret = (*env).NewByteArray(i * 2);
    jbyte faultInformation[10 * 2];

    vendingMachineProtocol.getFaultInformation((unsigned char*)faultInformation);
    (*env).SetByteArrayRegion(ret, 0, i * 2, faultInformation);
    return ret;
}

extern "C"

jint
Java_vmc_machine_impl_kubota_KubotaImpl_getFaultInformationCount(
        JNIEnv *env,
        jobject  /* this */) {
    return vendingMachineProtocol.getFaultInformationCount();
}

extern "C"

jbyteArray
Java_vmc_machine_impl_kubota_KubotaImpl_getVendingMachineNumber(
        JNIEnv *env,
        jobject  /* this */) {
    jbyteArray ret = (*env).NewByteArray(4);
    jbyte vMN[4];

    vendingMachineProtocol.getVendingMachineNumber((unsigned char*)vMN);
    (*env).SetByteArrayRegion(ret, 0, 4, vMN);
    return ret;
}


