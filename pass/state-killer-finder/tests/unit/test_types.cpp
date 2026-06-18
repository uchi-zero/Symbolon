#include "StateKillerFinder.h"
#include <string>
#include <symbolon/TestCheck.h>

int main() {
  using namespace statekillerfinder;
  Finding f{"foo.c", "main", 42, "call", "printf", Category::ExternCallSymArg};
  SYMBOLON_CHECK(f.file == "foo.c");
  SYMBOLON_CHECK(f.function == "main");
  SYMBOLON_CHECK(f.line == 42);
  SYMBOLON_CHECK(f.opcode == "call");
  SYMBOLON_CHECK(f.callee == "printf");
  SYMBOLON_CHECK(f.category == Category::ExternCallSymArg);

  Options o;
  SYMBOLON_CHECK(o.enabled.empty());
  SYMBOLON_CHECK(o.include_system == false);
  SYMBOLON_CHECK(o.pretty == false);

  for (Category c :
       {Category::ProgramTerminator, Category::UnreachableInstruction,
        Category::UnsupportedInstruction, Category::UnimplementedIntrinsic,
        Category::ExternCallSymArg, Category::ExternCallSymPtr,
        Category::ExternCallIndirect, Category::InlineAsm,
        Category::IndirectBranch, Category::SymbolicAllocSize,
        Category::SymbolicVectorIndex, Category::CallArityMismatch,
        Category::FreeOfNonHeap}) {
    std::string s{categoryName(c)};
    SYMBOLON_CHECK(!s.empty());
  }

  // Severity mapping is total. Pinned to --external-calls=concrete:
  // InlineAsm is Likely under concrete (would be Definite under none).
  SYMBOLON_CHECK(severityOf(Category::ProgramTerminator) == Severity::Definite);
  SYMBOLON_CHECK(severityOf(Category::UnreachableInstruction) ==
                 Severity::Definite);
  SYMBOLON_CHECK(severityOf(Category::UnsupportedInstruction) ==
                 Severity::Definite);
  SYMBOLON_CHECK(severityOf(Category::UnimplementedIntrinsic) ==
                 Severity::Definite);
  SYMBOLON_CHECK(severityOf(Category::ExternCallSymArg) == Severity::Likely);
  SYMBOLON_CHECK(severityOf(Category::ExternCallSymPtr) == Severity::Likely);
  SYMBOLON_CHECK(severityOf(Category::ExternCallIndirect) ==
                 Severity::Conditional);
  SYMBOLON_CHECK(severityOf(Category::InlineAsm) == Severity::Likely);
  SYMBOLON_CHECK(severityOf(Category::IndirectBranch) == Severity::Conditional);
  SYMBOLON_CHECK(severityOf(Category::SymbolicAllocSize) == Severity::Likely);
  SYMBOLON_CHECK(severityOf(Category::SymbolicVectorIndex) ==
                 Severity::Definite);
  SYMBOLON_CHECK(severityOf(Category::CallArityMismatch) == Severity::Definite);
  SYMBOLON_CHECK(severityOf(Category::FreeOfNonHeap) == Severity::Definite);

  // Severity names round-trip.
  SYMBOLON_CHECK(std::string{severityName(Severity::Definite)} == "definite");
  SYMBOLON_CHECK(std::string{severityName(Severity::Likely)} == "likely");
  SYMBOLON_CHECK(std::string{severityName(Severity::Conditional)} ==
                 "conditional");

  return 0;
}
