#define DEBUG_TYPE "PrintFunc"

#include <iostream>
#include <map>
#include <sstream>

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

struct FunctionInfo {
  string filename;
  string name;
  string type;
  unsigned start_line;
  unsigned end_line;
};

vector<FunctionInfo> function_infos;

class PrintFunc : public ModulePass {
public:
  static char ID;

  PrintFunc() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    LLVMContext &ctx = M.getContext();
    Module *Mod = &M;

    IRBuilder<> IRB(M.getContext());

    for (Function &F : M) {
      if (F.size() == 0) {
        continue;
      }

      string function_name = demangle(F.getName().str());

      // Get Function type
      FunctionType *FTy = F.getFunctionType();
      Type *RetTy = FTy->getReturnType();

      string ret_ty_str;
      raw_string_ostream ss(ret_ty_str);
      RetTy->print(ss);

      vector<string> arg_ty_strs(FTy->getNumParams());

      for (unsigned i = 0; i < FTy->getNumParams(); i++) {
        Type *ArgTy = FTy->getParamType(i);
        raw_string_ostream ss(arg_ty_strs[i]);
        ArgTy->print(ss);
      }

      // Make function type string
      stringstream ss_func_ty;
      for (unsigned i = 0; i < arg_ty_strs.size(); i++) {
        ss_func_ty << arg_ty_strs[i] << " -> ";
      }
      ss_func_ty << ret_ty_str;
      const string func_ty_str = ss_func_ty.str();

      // Find the range of function definition
      unsigned start_line = UINT32_MAX;
      unsigned end_line = 0;

      for (auto &BB : F) {
        for (auto &I : BB) {
          const DebugLoc &Loc = I.getDebugLoc();
          if (Loc) {
            unsigned line = Loc.getLine();
            if (line < start_line) {
              start_line = line;
            }
            if (line > end_line) {
              end_line = line;
            }
          }
        }
      }

      DISubprogram *subp = F.getSubprogram();
      if (!subp) {
        continue;
      }

      string filename = subp->getDirectory().str();
      filename += "/";
      filename += subp->getFilename().str();

      function_infos.push_back(
          {filename, function_name, func_ty_str, start_line, end_line});
    }

    cout << "[\n";
    for (FunctionInfo &info : function_infos) {
      cout << "{"
           << "\"filename\": \"" << info.filename << "\", "
           << "\"name\": \"" << info.name << "\", "
           << "\"type\": \"" << info.type << "\", "
           << "\"start_line\": " << info.start_line << ", "
           << "\"end_line\": " << info.end_line << "},\n";
    }
    cout << "]\n";

    return true;
  }
};

char PrintFunc::ID = 0;

static RegisterPass<PrintFunc> X("PrintFunc", "PrintFunc Pass", false, false);

static void registerPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new PrintFunc());
}

static RegisterStandardPasses
    RegisterPassOpt(PassManagerBuilder::EP_ModuleOptimizerEarly, registerPass);

static RegisterStandardPasses
    RegisterPassO0(PassManagerBuilder::EP_EnabledOnOptLevel0, registerPass);