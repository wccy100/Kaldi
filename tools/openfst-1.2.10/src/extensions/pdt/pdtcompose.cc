// pdtcompose.cc

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2005-2010 Google, Inc.
// Author: riley@google.com (Michael Riley)
//
// \file
// Composes a PDT and an FST.
//

#include <vector>
using std::vector;
#include <utility>
using std::pair; using std::make_pair;

#include <fst/util.h>
#include <fst/extensions/pdt/pdtscript.h>
#include <fst/script/connect.h>

DEFINE_string(pdt_parentheses, "", "PDT parenthesis label pairs.");
DEFINE_bool(left_pdt, true, "1st arg is PDT (o.w. 2nd arg).");
DEFINE_bool(connect, true, "Trim output");

int main(int argc, char **argv) {
  namespace s = fst::script;

  string usage = "Compose a PDT and an FST.\n\n  Usage: ";
  usage += argv[0];
  usage += " in.pdt in.fst [out.pdt]\n";
  usage += " in.fst in.pdt [out.pdt]\n";

  std::set_new_handler(FailedNewHandler);
  SetFlags(usage.c_str(), &argc, &argv, true);
  if (argc < 3 || argc > 4) {
    ShowUsage();
    return 1;
  }

  string in1_name = strcmp(argv[1], "-") == 0 ? "" : argv[1];
  string in2_name = strcmp(argv[2], "-") == 0 ? "" : argv[2];
  string out_name = argc > 3 ? argv[3] : "";

  if (in1_name.empty() && in2_name.empty()) {
    LOG(ERROR) << argv[0] << ": Can't take both inputs from standard input.";
    return 1;
  }

  s::FstClass *ifst1 = s::FstClass::Read(in1_name);
  if (!ifst1) return 1;
  s::FstClass *ifst2 = s::FstClass::Read(in2_name);
  if (!ifst2) return 1;

  if (FLAGS_pdt_parentheses.empty()) {
    LOG(ERROR) << argv[0] << ": No PDT parenthesis label pairs provided";
    return 1;
  }

  vector<pair<int64, int64> > parens;
  fst::ReadLabelPairs(FLAGS_pdt_parentheses, &parens, false);

  s::VectorFstClass ofst(ifst1->ArcType());
  fst::ComposeOptions copts(false);

  s::PdtCompose(*ifst1, *ifst2, parens, &ofst, copts, FLAGS_left_pdt);

  if (FLAGS_connect)
    s::Connect(&ofst);
  ofst.Write(out_name);

  return 0;
}
