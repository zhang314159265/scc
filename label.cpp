#include "label.h"
#include "emitter.h"

int Label::next_no_ = 0;

void Label::emit_if_used(Emitter* emitter) {
  if (used()) {
    emitter->emitLabel(this);
  }
}

void Label::emit(Emitter* emitter) {
  use();
  emit_if_used(emitter);
}
