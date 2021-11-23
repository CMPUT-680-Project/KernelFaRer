; ModuleID = 'test.cc'
source_filename = "test.cc"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@v = dso_local global float 0.000000e+00, align 4, !dbg !0
@o = dso_local local_unnamed_addr global i32 0, align 4, !dbg !6

; Function Attrs: nofree norecurse nounwind uwtable mustprogress
define dso_local void @_Z4testPKfPfS1_ifff(float* nocapture readonly %A, float* nocapture %B, float* nocapture readnone %C, i32 %n, float %a, float %b, float %c) local_unnamed_addr #0 !dbg !17 {
entry:
  call void @llvm.dbg.value(metadata float* %A, metadata !24, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata float* %B, metadata !25, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata float* %C, metadata !26, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata i32 %n, metadata !27, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata float %a, metadata !28, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata float %b, metadata !29, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata float %c, metadata !30, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata i32 0, metadata !31, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata i32 undef, metadata !31, metadata !DIExpression()), !dbg !34
  call void @llvm.dbg.value(metadata i32 0, metadata !32, metadata !DIExpression()), !dbg !35
  %add8 = fadd float %a, %b
  %add9 = fadd float %add8, %c
  call void @llvm.dbg.value(metadata i32 0, metadata !32, metadata !DIExpression()), !dbg !35
  call void @llvm.dbg.value(metadata i32 undef, metadata !31, metadata !DIExpression()), !dbg !34
  %cmp1.not24 = icmp slt i32 %n, 0, !dbg !36
  br i1 %cmp1.not24, label %for.cond.cleanup, label %for.body.lr.ph, !dbg !38

for.body.lr.ph:                                   ; preds = %entry
  %0 = add nuw i32 %n, 1, !dbg !38
  %wide.trip.count = zext i32 %0 to i64, !dbg !36
  br label %for.body, !dbg !38

for.cond.cleanup:                                 ; preds = %for.body, %entry
  ret void, !dbg !39

for.body:                                         ; preds = %for.body.lr.ph, %for.body
  %indvars.iv = phi i64 [ 0, %for.body.lr.ph ], [ %indvars.iv.next, %for.body ]
  call void @llvm.dbg.value(metadata i64 %indvars.iv, metadata !32, metadata !DIExpression()), !dbg !35
  %1 = shl nuw nsw i64 %indvars.iv, 1, !dbg !40
  %2 = add nsw i64 %1, -1, !dbg !42
  %arrayidx = getelementptr inbounds float, float* %A, i64 %2, !dbg !43
  %3 = load float, float* %arrayidx, align 4, !dbg !43, !tbaa !44
  %mul2 = fmul float %3, 3.000000e+00, !dbg !48
  %4 = or i64 %1, 1, !dbg !49
  %arrayidx5 = getelementptr inbounds float, float* %A, i64 %4, !dbg !50
  %5 = load float, float* %arrayidx5, align 4, !dbg !50, !tbaa !44
  %mul6 = fmul float %5, 2.000000e+00, !dbg !51
  %add7 = fadd float %mul2, %mul6, !dbg !52
  %6 = load volatile float, float* @v, align 4, !dbg !53, !tbaa !44
  %add10 = fadd float %add9, %6, !dbg !54
  %mul11 = fmul float %add10, 2.500000e-01, !dbg !55
  %add12 = fadd float %add7, %mul11, !dbg !56
  %fneg = fneg float %add12, !dbg !57
  %arrayidx14 = getelementptr inbounds float, float* %B, i64 %indvars.iv, !dbg !58
  store float %fneg, float* %arrayidx14, align 4, !dbg !59, !tbaa !44
  call void @llvm.dbg.value(metadata i32 undef, metadata !31, metadata !DIExpression(DW_OP_plus_uconst, 1, DW_OP_stack_value)), !dbg !34
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1, !dbg !60
  call void @llvm.dbg.value(metadata i64 %indvars.iv.next, metadata !32, metadata !DIExpression()), !dbg !35
  call void @llvm.dbg.value(metadata i32 undef, metadata !31, metadata !DIExpression()), !dbg !34
  %exitcond = icmp eq i64 %indvars.iv.next, %wide.trip.count, !dbg !36
  br i1 %exitcond, label %for.cond.cleanup, label %for.body, !dbg !38, !llvm.loop !61
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { nofree norecurse nounwind uwtable mustprogress "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!12, !13, !14, !15}
!llvm.ident = !{!16}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "v", scope: !2, file: !3, line: 1, type: !10, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !3, producer: "clang version 13.0.0 (https://github.com/Icohedron/KernelFaRer e2084b80bcda6592fc90130069f22065fb1c203c)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "test.cc", directory: "/home/icohedron/workspace/KernelFaRer/test-progs")
!4 = !{}
!5 = !{!0, !6}
!6 = !DIGlobalVariableExpression(var: !7, expr: !DIExpression())
!7 = distinct !DIGlobalVariable(name: "o", scope: !2, file: !3, line: 2, type: !8, isLocal: false, isDefinition: true)
!8 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !9)
!9 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!10 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !11)
!11 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!12 = !{i32 7, !"Dwarf Version", i32 4}
!13 = !{i32 2, !"Debug Info Version", i32 3}
!14 = !{i32 1, !"wchar_size", i32 4}
!15 = !{i32 7, !"uwtable", i32 1}
!16 = !{!"clang version 13.0.0 (https://github.com/Icohedron/KernelFaRer e2084b80bcda6592fc90130069f22065fb1c203c)"}
!17 = distinct !DISubprogram(name: "test", linkageName: "_Z4testPKfPfS1_ifff", scope: !3, file: !3, line: 3, type: !18, scopeLine: 4, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !23)
!18 = !DISubroutineType(types: !19)
!19 = !{null, !20, !22, !22, !9, !11, !11, !11}
!20 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64)
!21 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !11)
!22 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!23 = !{!24, !25, !26, !27, !28, !29, !30, !31, !32}
!24 = !DILocalVariable(name: "A", arg: 1, scope: !17, file: !3, line: 3, type: !20)
!25 = !DILocalVariable(name: "B", arg: 2, scope: !17, file: !3, line: 3, type: !22)
!26 = !DILocalVariable(name: "C", arg: 3, scope: !17, file: !3, line: 3, type: !22)
!27 = !DILocalVariable(name: "n", arg: 4, scope: !17, file: !3, line: 3, type: !9)
!28 = !DILocalVariable(name: "a", arg: 5, scope: !17, file: !3, line: 3, type: !11)
!29 = !DILocalVariable(name: "b", arg: 6, scope: !17, file: !3, line: 3, type: !11)
!30 = !DILocalVariable(name: "c", arg: 7, scope: !17, file: !3, line: 3, type: !11)
!31 = !DILocalVariable(name: "j", scope: !17, file: !3, line: 5, type: !9)
!32 = !DILocalVariable(name: "i", scope: !33, file: !3, line: 9, type: !9)
!33 = distinct !DILexicalBlock(scope: !17, file: !3, line: 9, column: 2)
!34 = !DILocation(line: 0, scope: !17)
!35 = !DILocation(line: 0, scope: !33)
!36 = !DILocation(line: 9, column: 20, scope: !37)
!37 = distinct !DILexicalBlock(scope: !33, file: !3, line: 9, column: 2)
!38 = !DILocation(line: 9, column: 2, scope: !33)
!39 = !DILocation(line: 14, column: 1, scope: !17)
!40 = !DILocation(line: 11, column: 19, scope: !41)
!41 = distinct !DILexicalBlock(scope: !37, file: !3, line: 9, column: 31)
!42 = !DILocation(line: 11, column: 21, scope: !41)
!43 = !DILocation(line: 11, column: 16, scope: !41)
!44 = !{!45, !45, i64 0}
!45 = !{!"float", !46, i64 0}
!46 = !{!"omnipotent char", !47, i64 0}
!47 = !{!"Simple C++ TBAA"}
!48 = !DILocation(line: 11, column: 14, scope: !41)
!49 = !DILocation(line: 11, column: 36, scope: !41)
!50 = !DILocation(line: 11, column: 31, scope: !41)
!51 = !DILocation(line: 11, column: 29, scope: !41)
!52 = !DILocation(line: 11, column: 25, scope: !41)
!53 = !DILocation(line: 11, column: 63, scope: !41)
!54 = !DILocation(line: 11, column: 61, scope: !41)
!55 = !DILocation(line: 11, column: 48, scope: !41)
!56 = !DILocation(line: 11, column: 40, scope: !41)
!57 = !DILocation(line: 11, column: 10, scope: !41)
!58 = !DILocation(line: 11, column: 3, scope: !41)
!59 = !DILocation(line: 11, column: 8, scope: !41)
!60 = !DILocation(line: 9, column: 26, scope: !37)
!61 = distinct !{!61, !38, !62, !63}
!62 = !DILocation(line: 13, column: 2, scope: !33)
!63 = !{!"llvm.loop.mustprogress"}
