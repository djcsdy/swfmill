#ifndef SWF_IDITEMS_H
#define SWF_IDITEMS_H

#include <SWFIdItem.h>

namespace SWF {

class StackItem : public IdItem<StackItem> {};
class Style : public IdItem<Style> {};
class NamespaceConstant : public IdItem<NamespaceConstant> {};
class MultinameConstant : public IdItem<MultinameConstant> {};
//class OpCode : public IdItem<OpCode> {};

}
#endif

