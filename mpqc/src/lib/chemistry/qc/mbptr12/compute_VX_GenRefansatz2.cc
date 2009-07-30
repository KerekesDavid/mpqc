//
// compute_VXB_GenRefansatz2.cc
//
// Copyright (C) 2008 Martin Torheyden
//
// Author: Martin Torheyden <mtorhey@vt.edu>
//
// This file is part of the SC Toolkit.
//
// The SC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The SC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#include <chemistry/qc/mbptr12/r12int_eval.h>
#include <chemistry/qc/mbptr12/creator.h>
#include <chemistry/qc/mbptr12/container.h>
#include <chemistry/qc/mbptr12/compute_tbint_tensor.h>
#include <chemistry/qc/mbptr12/contract_tbint_tensor.h>

using namespace std;
using namespace sc;

void R12IntEval::contrib_to_VX_GenRefansatz2_() {
  if (evaluated_)
    return;

  Ref<R12IntEval> thisref(this);
  Timer timer("General reference VXB intermediate evaluator");

  if(r12info()->ansatz()->projector() != LinearR12::Projector_2) {
    throw InputError("R12IntEval::contrib_to_VXB_GenRefansatz2_() -- this routine works only in combination with LinearR12::Projector_2.",__FILE__,__LINE__);
  }

  for(int s=0; s<nspincases2(); s++) {
    using namespace sc::LinearR12;
    const SpinCase2 spincase2 = static_cast<SpinCase2>(s);
    const SpinCase1 spin1 = case1(spincase2);
    const SpinCase1 spin2 = case2(spincase2);
    Ref<SingleRefInfo> refinfo = r12info()->refinfo();

    if (dim_oo(spincase2).n() == 0)
      continue;

    const Ref<OrbitalSpace>& gg1_space = ggspace(spin1);
    const Ref<OrbitalSpace>& gg2_space = ggspace(spin2);
    const Ref<OrbitalSpace>& orbs1 = refinfo->orbs(spin1);
    const Ref<OrbitalSpace>& orbs2 = refinfo->orbs(spin2);
    const Ref<OrbitalSpace>& GG1_space = GGspace(spin1);
    const Ref<OrbitalSpace>& GG2_space = GGspace(spin2);
    const Ref<OrbitalSpace>& abs1 = r12info()->abs_space();
    const Ref<OrbitalSpace>& abs2 = r12info()->abs_space();
    const Ref<OrbitalSpace>& cabs1 = r12info()->ribs_space(spin1);
    const Ref<OrbitalSpace>& cabs2 = r12info()->ribs_space(spin2);

    const bool gg1_eq_gg2 = (gg1_space==gg2_space);
    const bool GG1_eq_GG2 = (GG1_space==GG2_space);
    const bool orbs1_eq_orbs2 = (orbs1==orbs2);

    if(gg1_eq_gg2 ^ GG1_eq_GG2) {
      throw ProgrammingError("R12IntEval::contrib_to_VXB_GenRefansatz2_() -- gg1 and gg2 space must be of the same structure as GG1 and GG2 space",__FILE__,__LINE__);
    }

    // are particles 1 and 2 equivalent?
    const bool part1_equiv_part2 =  spincase2 != AlphaBeta || orbs1_eq_orbs2;
    ExEnv::out0() << "part1_equiv_part2 = " << ((part1_equiv_part2) ? "true" : "false") << endl;
    // Need to antisymmetrize 1 and 2
    const bool antisymmetrize = spincase2 != AlphaBeta;

    // some transforms can be skipped if gg1/gg2 is a subset of GG1/GG2
    // for now it's always true since can only use pq products to generate geminals
    const bool gg12_in_GG12 = true;

    Ref<TwoParticleContraction> tpcontract;
    const ABSMethod absmethod = r12info()->abs_method();
    tpcontract = new CABS_OBS_Contraction(refinfo->orbs(spin1)->rank());

    const bool cabs_method = (absmethod ==  LinearR12::ABS_CABS ||
                    absmethod == LinearR12::ABS_CABSPlus);

    Ref<OrbitalSpace> rispace1 = r12info()->ribs_space();
    Ref<OrbitalSpace> rispace2 = r12info()->ribs_space();

    /// computing intermediate V
    Timer Vtimer("General reference V intermediate evaluator");

    //
    // "OBS" term:
    // V_{p_2 p_3}^{r s} -= \frac{1}{2}\bar{g}^{q_2 q_3}_{p_2 p_3}\bar{r}^{r s}_{q_2 q_3}
    //
    {
      std::vector<std::string> tforms_f12;
      {
        R12TwoBodyIntKeyCreator tform_creator(
                            r12info()->moints_runtime4(),
                            GG1_space,orbs1,GG2_space,orbs2,
                            r12info()->corrfactor(),true
                            );
        fill_container(tform_creator,tforms_f12);
      }
      std::vector<std::string> tforms;
      {
        const std::string tform_key = ParsedTwoBodyFourCenterIntKey::key(gg1_space->id(),gg2_space->id(),
                                                                         orbs1->id(),orbs2->id(),
                                                                         std::string("ERI"),
                                                                         std::string(TwoBodyIntLayout::b1b2_k1k2));
        tforms.push_back(tform_key);
      }
      contract_tbint_tensor<true,false>
          (V_[s], corrfactor()->tbint_type_f12(), corrfactor()->tbint_type_eri(),
           -1.0,
           GG1_space, GG2_space, orbs1, orbs2,
           gg1_space, gg2_space, orbs1, orbs2,
           spincase2!=AlphaBeta, tforms_f12, tforms);
    }

    //
    // "CABS" term:
    // V_{p_2 p_3}^{r s} -= \bar{g}^{q_3 \alpha'}_{p_2 p_3} \gamma^{q_2}_{q_3} \bar{r}^{r s}_{q_2 \alpha'}
    //
    {
      Ref<OrbitalSpace> gamma_p_p1;
      Ref<OrbitalSpace> gamma_p_p2;
      gamma_p_p1 = gamma_p_p(spin1);
      gamma_p_p2 = gamma_p_p(spin2);
      std::vector<std::string> tforms_f12;
      {
        R12TwoBodyIntKeyCreator tform_creator(
                            r12info()->moints_runtime4(),
                            GG1_space,gamma_p_p1,GG2_space,cabs2,
                            r12info()->corrfactor(),true
                            );
        fill_container(tform_creator,tforms_f12);
      }
      std::vector<std::string> tforms;
      {
        const std::string tform_key = ParsedTwoBodyFourCenterIntKey::key(gg1_space->id(),gg2_space->id(),
                                                                         orbs1->id(),cabs2->id(),
                                                                         std::string("ERI"),
                                                                         std::string(TwoBodyIntLayout::b1b2_k1k2));
        tforms.push_back(tform_key);
      }
      contract_tbint_tensor<true,false>(
          V_[s], corrfactor()->tbint_type_f12(), corrfactor()->tbint_type_eri(),
          -1.0,
          GG1_space, GG2_space,
          gamma_p_p1,cabs2,
          gg1_space, gg2_space,
          orbs1,cabs2,
          spincase2!=AlphaBeta, tforms_f12, tforms);

      if(spincase2==AlphaBeta) {
        std::vector<std::string> tforms_f12;
        {
          R12TwoBodyIntKeyCreator tform_creator(
                              r12info()->moints_runtime4(),
                              GG1_space,cabs1,GG2_space,gamma_p_p2,
                              r12info()->corrfactor(),true
                              );
          fill_container(tform_creator,tforms_f12);
        }
        std::vector<std::string> tforms;
        {
          const std::string tform_key = ParsedTwoBodyFourCenterIntKey::key(gg1_space->id(),gg2_space->id(),
                                                                           cabs1->id(),orbs2->id(),
                                                                           std::string("ERI"),
                                                                           std::string(TwoBodyIntLayout::b1b2_k1k2));
          tforms.push_back(tform_key);
        }
        contract_tbint_tensor<true,false>(
            V_[s], corrfactor()->tbint_type_f12(), corrfactor()->tbint_type_eri(),
            -1.0,
            GG1_space, GG2_space,
            cabs1,gamma_p_p2,
            gg1_space, gg2_space,
            cabs1,orbs2,
            spincase2!=AlphaBeta, tforms_f12, tforms);
      }
    }

    if (!antisymmetrize && part1_equiv_part2) {
        symmetrize<false>(V_[s],V_[s],GG1_space,gg1_space);
    }
    if (debug_ >= DefaultPrintThresholds::O4) {
        V_[s].print(prepend_spincase(static_cast<SpinCase2>(s),"V(diag+OBS+ABS) contribution").c_str());
    }

    Vtimer.exit();

    Timer Xtimer("General reference X intermediate evaluator");

    { /// OBS Term
      std::vector<std::string> tforms_f12;
      {
        R12TwoBodyIntKeyCreator tform_creator(
                            r12info()->moints_runtime4(),
                            GG1_space,orbs1,GG2_space,orbs2,
                            r12info()->corrfactor(),true
                            );
        fill_container(tform_creator,tforms_f12);
      }
      contract_tbint_tensor<true,true>
          (X_[s], corrfactor()->tbint_type_f12(), corrfactor()->tbint_type_f12(),
           -1.0,
           GG1_space, GG2_space, orbs1, orbs2,
           GG1_space, GG2_space, orbs1, orbs2,
           spincase2!=AlphaBeta, tforms_f12, tforms_f12);
    }
    { /// CABS Term
      Ref<OrbitalSpace> gamma_p_p1;
      Ref<OrbitalSpace> gamma_p_p2;
      gamma_p_p1 = gamma_p_p(spin1);
      gamma_p_p2 = gamma_p_p(spin2);
      std::vector<std::string> tforms_bra_f12;
      {
        R12TwoBodyIntKeyCreator tform_creator(
                            r12info()->moints_runtime4(),
                            GG1_space,gamma_p_p1,GG2_space,cabs2,
                            r12info()->corrfactor(),true
                            );
        fill_container(tform_creator,tforms_bra_f12);
      }
      std::vector<std::string> tforms_ket_f12;
      {
        R12TwoBodyIntKeyCreator tform_creator(
                            r12info()->moints_runtime4(),
                            GG1_space,orbs1,GG2_space,cabs2,
                            r12info()->corrfactor(),true
                            );
        fill_container(tform_creator,tforms_ket_f12);
      }
      contract_tbint_tensor<true,true>
          (X_[s], corrfactor()->tbint_type_f12(), corrfactor()->tbint_type_f12(),
           -1.0,
           GG1_space, GG2_space,
           gamma_p_p1,cabs2,
           GG1_space, GG2_space,
           orbs1,cabs2,
           spincase2!=AlphaBeta,tforms_bra_f12,tforms_ket_f12
           );
      if(spincase2==AlphaBeta) {
        std::vector<std::string> tforms_bra_f12;
        {
          R12TwoBodyIntKeyCreator tform_creator(
                              r12info()->moints_runtime4(),
                              GG1_space,cabs1,GG2_space,gamma_p_p2,
                              r12info()->corrfactor(),true
                              );
          fill_container(tform_creator,tforms_bra_f12);
        }
        std::vector<std::string> tforms_ket_f12;
        {
          R12TwoBodyIntKeyCreator tform_creator(
                              r12info()->moints_runtime4(),
                              GG1_space,cabs1,GG2_space,orbs2,
                              r12info()->corrfactor(),true
                              );
          fill_container(tform_creator,tforms_ket_f12);
        }
        contract_tbint_tensor<true,true>
            (X_[s], corrfactor()->tbint_type_f12(), corrfactor()->tbint_type_f12(),
             -1.0,
             GG1_space, GG2_space,
             cabs1,gamma_p_p2,
             GG1_space, GG2_space,
             cabs1,orbs2,
             spincase2!=AlphaBeta,tforms_bra_f12,tforms_ket_f12);
      }
    }

    if (!antisymmetrize && part1_equiv_part2) {
        symmetrize<false>(X_[s],X_[s],GG1_space,GG1_space);
    }
    if (debug_ >= DefaultPrintThresholds::O4) {
        X_[s].print(prepend_spincase(static_cast<SpinCase2>(s),"X(diag+OBS+ABS) contribution").c_str());
    }

    Xtimer.exit();
  }

  timer.exit();
}