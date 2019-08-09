/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil  -*- */
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 expandtab: */

//============================================================================
// Copyright (c) 2011, All Right Reserved, Nimbus Automation Technologies
//
// file:      ESLPass.h
// author:    Zhenghua Gu
// purpose:   The whole backend of ESL (header file)
// revision history:
//============================================================================

#ifndef BACKEND_H_
#define BACKEND_H_

#include <tcl.h>

#include "llvm/Pass.h"
//#include "llvm/IR/InstVisitor.h"
//#include "llvm/IR/Dominators.h"
//#include "llvm/Target/TargetMachine.h"
//#include "llvm/Analysis/AliasAnalysis.h"
//#include "llvm/Analysis/BasicAliasAnalysis.h"
//#include "llvm/Analysis/LoopInfo.h"
//#include "llvm/Analysis/CallGraph.h"
//#include "llvm/Analysis/ScalarEvolution.h"
//#include "llvm/Analysis/TargetTransformInfo.h"
//#include "llvm/Analysis/LoopInfo.h"

#include "cdfg/dg_builder.h"
#include "device/device.h"
using namespace llvm;
using namespace std;
namespace esl {

class RemoveStruct {
	public:
	RemoveStruct(Module* M) : module_(M) {}
	  ~RemoveStruct() {}
	public:
	  void RemoveStructMain();
	private:
    Module* module_;
    typedef struct TypeWidth {
        Type* type;
        int width;
        TypeWidth(Type* T, int W) {
          type = T;
          width = W;
        }
    }type_width;
	  //Change BC for replaceStruct() better
	  void changeBC();
	  void splitLoad();
	  void splitStore();
	  void splitBitCast();
	  void splitGetelementPtr();
	  void removeLifetime();
	  void changeBitCast();
	  void BitCastLoad(Instruction* instr);
	  void BitCastGetelementPtr(Instruction* instr);
	  void structMemoryArrange(Type* val_ty, vector<int> val_list, map<vector<int>, type_width> &val_map, int &use_width);
    //void structMemoryArrange(Type* val_ty, vector<int> val_list, map<vector<int>, int> &val_map, int &use_width);
    void structMemoryArrangePrint (map<vector<int>, type_width> val_map);
	  void StructAlign(Type* val_ty, int &align);
	  void ArrayAlign(Type* val_ty, int &align);
	  //void structMapType(Type* val_ty, vector<int> &val_list, map<vector<int>, Type*> &val_map);
	  //void structArrayMapType(Type* val_ty, vector<int> &val_list, map<vector<int>, Type*> &val_map);
	  void structArrangeTestFlag(Type* val_ty, int actual_width, bool &flag, int &use_width);
	  void changeMemcpy();
	  //Replace struct in BC
	  void replaceStruct();
	  //Resolve GlobalVariable
	  void resolveGlobalVar();
	  //Resolve GlobalVariable struct part
	  void resolveStructVar(Value * const_gv, GlobalVariable* gv, string &gv_name);
	  void resolveArrayVar(Value * const_gv, GlobalVariable* gv, string &gv_name);
	  //Replace Instruction which contains struct type
	  void replaceStructInstr();
	  //Replace GlobalVariable which contained struct by new resolve globalVariable (keymap_)
	  void GlobalVarGetelementptr();
	  void replaceGlobalvarGetelementprt(Instruction* instr);
	  //Replace Alloca which contained Struct & replace related instruction
	  void AllocaInstStruct();
	  //Replace Alloca which contained Array & replace related instruction
	  void replaceAllocaInstrArray();
	  void replaceAllocaStruct(Instruction* allocaInst,SmallVector<Instruction*,128> *WorkListGetEStore);
	  void DoubleGetElementPtr();
	  void replaceDoubleGetElementPtr(Instruction* instr);
	  void replaceAllocaRelatedInstrStruct(map<std::string, Value*>);
	  void replaceAllocaRelatedInstrArray(map<std::string, Value*>);
	  bool findStruct();

	  void memcpy(Instruction* instr);
	  void memcpySecondConstantExpr(Value* memcpy_from,Value* memcpy_to, Instruction* instr);
	  void memcpyToConstantExpr(Value* memcpy_from,Value* memcpy_to, Instruction* instr);
	  void memcpyToConstantExprGetelementPtr(ConstantExpr* instrExpr, ConstantExpr* instrExpr_2);
	  void memcpyToGetElementPtrInst(Value* memcpy_from, GetElementPtrInst* intrinsic_to, Instruction* instr);
	  Value* memcpyParm(Value* parameter);
	  //remove struct function
	  void removeFun();
	  void traversing();
	  BasicBlock* returnBasicBlock();
	  void structGlobalVarible();
	  void processStruct(Value * const_gv, GlobalVariable* gv, string &gv_name);
	  void processArray(Value * const_gv, GlobalVariable* gv, string &gv_name);
	  void processCall(llvm::Instruction * & );

	  vector<Value *>  valueInstr(const llvm::Instruction* );

	  vector<Value *>  valueExpr(const llvm::ConstantExpr* constExpr);
	  void bitwidthOptForRom();


	  void getgGlobalVar(const std::set<llvm::GlobalVariable *>& );
	private:
	  bool canReplaceMultiplyByConstantWithShiftAdd(
			uint64_t multiplierInput, int& exp1, int& exp2, bool& toAdd);
	private:

	  map<std::string /*Key*/, Value * /*Value*/> keymap_;
	  SmallVector<GlobalVariable*, 128> *WorkListGV = new SmallVector<GlobalVariable*, 128> ();
};

class ESLPass : public ModulePass {
public:
  ESLPass(raw_ostream &o) : ModulePass(ID), out_(o) {
    device_ = NULL;
    clock_period_ = 0.0;
  }

  ~ESLPass() { delete device_; }

	virtual StringRef getPassName() const { return "The back end pass"; }

	virtual bool doInitialization(Module &m);

	bool runOnModule(Module &m);

	virtual bool doFinalization(Module &m) { return false; }
  
	Device *getDevice() const { return device_; }
	float getClockPeriod() const { return clock_period_; }

private:
  //bool reviseModForMemAnal(Module &m);

private:
 void getAnalysisUsage(AnalysisUsage &AU) const {
   AU.addRequired<DGBuilder>();
   //AU.addRequired<AssumptionCacheTracker>();
   //AU.addRequired<DominatorTreeWrapperPass>();
   //AU.addRequired<AAResultsWrapperPass>(); 
   //AU.addRequired<BasicAAWrapperPass>(); 
   //AU.addRequired<LoopInfoWrapperPass>();
   //AU.addRequired<LiveVariableAnalysis>();
   //AU.addRequired<ScalarEvolutionWrapperPass>();
   //AU.addRequired<TargetTransformInfoWrapperPass>();
   //AU.setPreservesAll();
 }
private:
	raw_ostream &out_;
	static char ID;

private:
	Device *device_;
	float clock_period_;
};


} // end of namespace

#endif /* BACKEND_H_ */
