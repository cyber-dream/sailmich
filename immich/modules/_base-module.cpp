#include "_base-module.h"
using namespace Immich;

BaseModule::ApiStruct BaseModule::api;

BaseModule::BaseModule(QObject *parent)
    : QObject(parent){}

void BaseModule::setApi(const ApiStruct &pApiStruct) { api = pApiStruct; }
