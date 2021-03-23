#include "rollbacks/factory.h"
#include "rollbacks/exception.h"
#include "rollbacks/fiovb.h"
#include "rollbacks/generic.h"
#include "rollbacks/masked.h"

Rollback* RollbackFactory::makeRollback(RollbackMode mode) {
  switch (mode) {
    case RollbackMode::kBootloaderNone:
      return new Rollback();
      break;
    case RollbackMode::kUbootGeneric:
      return new GenericRollback();
      break;
    case RollbackMode::kUbootMasked:
      return new MaskedRollback();
      break;
    case RollbackMode::kFioVB:
      return new FiovbRollback();
      break;
    default:
      return new ExceptionRollback();
  }
}
