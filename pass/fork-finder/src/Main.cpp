#include "ForkFinder.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MemoryBuffer.h>
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
    IncludeImplicit("include-implicit",
                    cl::desc("emit DivByZero and OverflowCheck findings"));
static cl::opt<bool>
    IncludeSystem("include-system",
                  cl::desc("don't skip findings whose path looks like a system "
                           "header / KLEE runtime"));
static cl::opt<bool> IncludeNonLoop(
    "include-non-loop",
    cl::desc("emit every fork site regardless of source-level "
             "loop containment (LibraryLoop findings are emitted "
             "in both modes)"));
static cl::opt<bool> Pretty("pretty",
                            cl::desc("pretty-print JSON with 2-space indent"));

static void printVersion(raw_ostream &os) { os << "fork-finder 0.1\n"; }

int main(int argc, char **argv) {
  // LLVM already registers --version; use SetVersionPrinter to override
  // it rather than declaring a second cl::opt<bool>("version", ...) which
  // triggers "Option 'version' registered more than once!" at startup.
  cl::SetVersionPrinter(printVersion);
  cl::ParseCommandLineOptions(argc, argv, "fork-finder\n");

  std::error_code ec;
  if (!std::filesystem::exists(InputFile.getValue(), ec) || ec) {
    errs() << "fork-finder: cannot open " << InputFile.getValue()
           << ": file not found\n";
    return 1;
  }

  LLVMContext ctx;
  SMDiagnostic err;
  std::unique_ptr<Module> mod = parseIRFile(InputFile, err, ctx);
  if (!mod) {
    err.print("fork-finder", errs());
    return 2;
  }

  forkfinder::Options opts;
  opts.include_implicit = IncludeImplicit;
  opts.include_system = IncludeSystem;
  opts.include_non_loop = IncludeNonLoop;
  opts.pretty = Pretty;

  forkfinder::ForkFinderPass pass(opts);

  PassBuilder pb;
  ModuleAnalysisManager mam;
  pb.registerModuleAnalyses(mam);
  pass.run(*mod, mam);

  auto json = forkfinder::renderReport(pass.findings(), opts, mod->getName());
  std::string text = opts.pretty ? json.dump(2) : json.dump();

  if (OutputFile.empty()) {
    outs() << text << "\n";
  } else {
    std::ofstream f(OutputFile);
    if (!f) {
      errs() << "fork-finder: cannot open " << OutputFile << "\n";
      return 1;
    }
    f << text << "\n";
  }
  return 0;
}
