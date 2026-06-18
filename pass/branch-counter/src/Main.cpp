#include "BranchCounter.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

using namespace llvm;

static cl::opt<std::string>
    InputFile(cl::Positional, cl::desc("<input.bc|input.ll>"), cl::Required);
static cl::opt<std::string>
    OutputFile("o", cl::desc("write JSON to <path> (default stdout)"),
               cl::value_desc("path"));
static cl::opt<bool>
    IncludeSystem("include-system",
                  cl::desc("don't skip branches whose path looks like a "
                           "system header / KLEE runtime"));
static cl::opt<bool> Pretty("pretty",
                            cl::desc("pretty-print JSON with 2-space indent"));

static void printVersion(raw_ostream &os) { os << "branch-counter 0.1\n"; }

int main(int argc, char **argv) {
  cl::SetVersionPrinter(printVersion);
  cl::ParseCommandLineOptions(argc, argv, "branch-counter\n");

  std::error_code ec;
  if (!std::filesystem::exists(InputFile.getValue(), ec) || ec) {
    errs() << "branch-counter: cannot open " << InputFile.getValue()
           << ": file not found\n";
    return 1;
  }

  LLVMContext ctx;
  SMDiagnostic err;
  std::unique_ptr<Module> mod = parseIRFile(InputFile, err, ctx);
  if (!mod) {
    err.print("branch-counter", errs());
    return 2;
  }

  branchcounter::Options opts;
  opts.include_system = IncludeSystem;
  opts.pretty = Pretty;

  auto branches = branchcounter::collectBranches(*mod, opts);
  auto json = branchcounter::renderReport(branches, mod->getName());
  std::string text = opts.pretty ? json.dump(2) : json.dump();

  if (OutputFile.empty()) {
    outs() << text << "\n";
  } else {
    std::ofstream f(OutputFile);
    if (!f) {
      errs() << "branch-counter: cannot open " << OutputFile << "\n";
      return 1;
    }
    f << text << "\n";
  }
  return 0;
}
