#define DEBUG_TYPE "PrintFunc"

#include <iostream>
#include <map>

#include "llvm/ADT/Statistic.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

/* PrintFunc.cpp
 *
 * Print at function entry point and return point
 * */

using namespace llvm;
using namespace std;

class PrintFunc : public ModulePass {
 public:
  static char ID;  // Pass identification, replacement for typeid

  // Fill out.

  PrintFunc() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    LLVMContext &ctx = M.getContext();
    Module *Mod = &M;

    IRBuilder<> IRB(M.getContext());

    for (auto &F : M) {
      if (F.size() == 0) {
        continue;
      }

      string function_name = demangle(F.getName().str());

      // If function_name has open bracket, truncate it.
      // size_t pos = function_name.find("(");
      // if (pos != string::npos) {
      //   function_name = function_name.substr(0, pos);
      // }

      DISubprogram *subp = F.getSubprogram();
      if (!subp) {
        continue;
      }

      const string dirname = subp->getDirectory().str();
      const string filename = subp->getFilename().str();


      printf("%s/%s %s\n", dirname.c_str(), filename.c_str(), function_name.c_str());
    }

    return true;
  }
};

char PrintFunc::ID = 0;

static RegisterPass<PrintFunc> X("PrintFunc", "PrintFunc Pass", false, false);

static void registerPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new PrintFunc());
}

static RegisterStandardPasses RegisterPassOpt(
    PassManagerBuilder::EP_ModuleOptimizerEarly, registerPass);

static RegisterStandardPasses RegisterPassO0(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registerPass);