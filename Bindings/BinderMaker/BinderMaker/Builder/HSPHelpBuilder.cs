﻿
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BinderMaker.Builder
{
    /// <summary>
    /// .hs 生成
    /// </summary>
    class HSPHelpBuilder : Builder
    {
        #region Templates
        const string HeaderTemplate = @"
;============================================================
; Lumino ヘルプファイル
;============================================================

%dll
Lumino

%ver
_LIBRARY_VERSION_

%date
_BUILD_TIME_

%author
lriki

%note
lumino.as をインクルードしてください

%type
拡張命令

%url
http://nnmy.sakura.ne.jp/

";

        const string FuncTemplate = @"
;-----------------------------------------------------------
;
;-----------------------------------------------------------
%index
_NAME_
_BRIEF_
%group
_GROUP_
%prm
_PRM_LIST_
_PRM_DETAIL_
%inst
_INST_
%href
_HREF_
";
        #endregion

        private OutputBuffer _output = new OutputBuffer();

        /// <summary>
        /// ビルド開始前(初期化)通知
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnInitialize()
        {
            // ヘッダ
            string t = HeaderTemplate.TrimStart();
            //t = t.Replace("_LIBRARY_VERSION_", CLManager.GetLibararyVersion());
            t = t.Replace("_BUILD_TIME_", DateTime.Today.ToString("yyyy/MM/dd"));
            _output.AppendWithIndent(t);
        }

        /// <summary>
        /// クラスor構造体 通知 (開始)
        /// </summary>
        /// <param name="classType"></param>
        /// <returns>false の場合このクラスの出力を無視する</returns>
        protected override bool OnClassLookedStart(CLClass classType)
        {

            return true;
        }

        /// <summary>
        /// クラスor構造体 通知 (終了)
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnClassLookedEnd(CLClass classType)
        {
        }

        /// <summary>
        /// プロパティ 通知
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnPropertyLooked(CLProperty prop)
        {
            // プロパティとして割り振られたメソッドは OnMethodLooked() ではなくこの OnPropertyLooked() にくる。
            // やりたいことは OnMethodLooked() と同じなのでそちらにまわす。
            if (prop.Getter != null) OnMethodLooked(prop.Getter);
            if (prop.Setter != null) OnMethodLooked(prop.Setter);
        }

        /// <summary>
        /// メソッド 通知
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnMethodLooked(CLMethod method)
        {
            if (!Context.CheckEnabled(method)) return;

            // 関数
            string allFuncText = "";

            // 基本
            string funcText = FuncTemplate.TrimStart();
            funcText = funcText.Replace("_NAME_", method.FuncDecl.OriginalFullName);
            funcText = funcText.Replace("_BRIEF_", TranslateComment(method.Document.OriginalBriefText));
            funcText = funcText.Replace("_INST_", TranslateComment(method.Document.OriginalDetailsText));
            funcText = funcText.Replace("_HREF_", "");
            funcText = funcText.Replace("_GROUP_", method.OwnerClass.OwnerModule.Document.OriginalGroup);

            // 引数リスト
            var paramsText = new OutputBuffer();
            foreach (var param in method.FuncDecl.Params)
            {
                paramsText.AppendCommad(param.Name);
            }
            if (paramsText.IsEmpty) paramsText.Append("　"); // 空なら全角スペースでごまかす
            funcText = funcText.Replace("_PRM_LIST_", paramsText.ToString());

            // インデント量の計算
            // [out] pp   : 説明
            // [in]  name : 説明
            // ^^^^^        … ioColumnWidth
            //       ^^^^   … nameColumnWidth
            int ioColumnWidth = 0;
            int nameColumnWidth = 0;
            foreach (var param in method.FuncDecl.Params)
            {
                // 名前部分はデフォルト引数も含んだ長さで考える
                int nameLen = param.Name.Length;
                if (!string.IsNullOrEmpty(param.OriginalDefaultValue))
                    nameLen += param.OriginalDefaultValue.Length + 2;   // +2 は () の分

                ioColumnWidth = Math.Max(ioColumnWidth, GetIOName(param.IOModifier).Length);
                nameColumnWidth = Math.Max(nameColumnWidth, nameLen);
            }
            ioColumnWidth += 2; // 前後の [ ] の分

            // 引数の1行説明
            string detailText = "";
            foreach (var param in method.FuncDecl.Params)
            {
                string name = param.Name;

                // デフォルト値がある場合は () を付けて表現
                if (!string.IsNullOrEmpty(param.OriginalDefaultValue))
                    name += "(" + param.OriginalDefaultValue + ")";

                detailText += string.Format("{0,-" + ioColumnWidth + "}", "[" + GetIOName(param.IOModifier) + "]");
                detailText += string.Format(" {0,-" + nameColumnWidth + "}", name);
                detailText += " : ";
                detailText += TranslateComment(param.Document.OriginalText);
                //detailText += "\n";

                // enum 型の場合は候補値も追加しておく
                CLEnum enumType = param.Type as CLEnum;
                if (enumType != null)
                {
                    string indent = string.Format(" {0,-" + (ioColumnWidth + 3 + nameColumnWidth + 3) + "}", " ");

                    foreach (var member in enumType.Members)
                    {
                        if (member.IsTerminator) continue;  // ターミネータは追加しない
                        detailText += indent;
                        detailText += member.OriginalName + OutputBuffer.NewLineCode;
                        detailText += indent + "    ";
                        detailText += member.Comment + OutputBuffer.NewLineCode;
                    }
                }
            }

            // 戻り値 (全て stat 扱い)
            if (method.FuncDecl.ReturnType != null && method.FuncDecl.ReturnType.IsResultCodeType)
            {
                if (method.FuncDecl.OriginalFullName == "LNError_GetLastErrorCode")
                {
                    detailText += OutputBuffer.NewLineCode;
                    detailText += "stat : エラーコード" + OutputBuffer.NewLineCode;
                    
                    CLEnum enumType = (CLEnum)method.FuncDecl.ReturnType;
                    foreach (var member in enumType.Members)
                    {
                        if (member.IsTerminator) continue;  // ターミネータは追加しない
                        detailText += "    ";
                        detailText += member.OriginalName + OutputBuffer.NewLineCode;
                        detailText += "        ";
                        detailText += member.Comment + OutputBuffer.NewLineCode;
                    }
                    
                    detailText += OutputBuffer.NewLineCode;
                }
                else
                {
                    detailText += OutputBuffer.NewLineCode;
                    detailText += "stat : エラーコード (エラーコードについては LNError_GetLastErrorCode を参照してください)";
                    detailText += OutputBuffer.NewLineCode;
                }
            }

            funcText = funcText.Replace("_PRM_DETAIL_", detailText);

            // サンプルコード
            //TestCode sampleCode;
            //if (_sampleCodeMap.TryGetValue(func.CPPName, out sampleCode) &&
            //    !sampleCode.IsSkipTest)
            //{
            //    funcText += "\n%sample\n" + sampleCode.ExtractedCode + "\n";
            //}

            allFuncText += funcText;
            _output.AppendWithIndent(allFuncText);
        }

        /// <summary>
        /// ファイルに出力するための最終文字列を生成する
        /// </summary>
        protected override string OnMakeOutoutFileText()
        {
            return _output.ToString();
        }

        /// <summary>
        /// 出力ファイルのエンコーディング
        /// </summary>
        protected override Encoding GetOutputEncoding() { return Encoding.GetEncoding(932); }


        public static string TranslateComment(string text)
        {
            text = text.Replace("関数", "命令");
            text = text.Replace("のポインタ", "");
            text = text.Replace("クラス", "モジュール");

            string doc = "";
            string[] lines = text.Replace("\r", "").Split('\n');
            foreach (var line in lines)
                doc += line.Trim() + OutputBuffer.NewLineCode;

            return doc;
        }

        public static string GetIOName(IOModifier m)
        {
            if (m == IOModifier.Out)
                return "out";
            return "in";
        }
    }

    
    /// <summary>
    /// 命令一覧
    /// </summary>
    class HSPFuncListBuilder : Builder
    {
        private struct DocLine
        {
            public string Name;
            public string Brief;
        }

        private List<DocLine> _oneClass;
        private OutputBuffer _output = new OutputBuffer();

        /// <summary>
        /// ビルド開始前(初期化)通知
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnInitialize()
        {
            _output.AppendLine("===============================================================================");
            _output.AppendLine(" Lumino HSP 命令一覧");
            _output.AppendLine("-------------------------------------------------------------------------------");
            _output.AppendLine("  このファイルは、本ライブラリがどのような機能を持っているかを示す概要です。");
            _output.AppendLine("  個々の命令の詳しい説明は、同梱の lumino.hs を HSP インストールフォルダ内の");
            _output.AppendLine("  「hsphelp」フォルダにコピーし、エディタから F1 キーヘルプにて参照してください。");
            _output.AppendLine("===============================================================================");
        }

        /// <summary>
        /// クラスor構造体 通知 (開始)
        /// </summary>
        /// <param name="classType"></param>
        /// <returns>false の場合このクラスの出力を無視する</returns>
        protected override bool OnClassLookedStart(CLClass classType)
        {
            _oneClass = new List<DocLine>();

            if (classType.IsStruct)
            {
                _oneClass.Add(
                    new DocLine()
                    {
                        Name = classType.OriginalName + "()",
                        Brief = classType.OriginalName + "型変数の作成" + OutputBuffer.NewLineCode
                    });
            }

            return true;
        }

        /// <summary>
        /// クラスor構造体 通知 (終了)
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnClassLookedEnd(CLClass classType)
        {
            _output.AppendLine("-------------------------------------------------------------------------------");
            _output.AppendLine(" ■ [{0}]  {1}", classType.OriginalName, HSPHelpBuilder.TranslateComment(classType.Document.OriginalBriefText));

            // 関数名カラムの最大文字数を求める
            int col1Len = 0;
            foreach (var line in _oneClass)
            {
                col1Len = Math.Max(col1Len, line.Name.Length);
            }

            // インデントを考慮し、テキストを作る
            foreach (var line in _oneClass)
            {
                _output.Append(string.Format("{0,-" + col1Len + "}  ", line.Name));
                _output.Append(line.Brief);
            }
            _output.NewLine();
        }
        
        /// <summary>
        /// プロパティ 通知
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnPropertyLooked(CLProperty prop)
        {
            if (prop.Setter != null) OnMethodLooked(prop.Setter);
            if (prop.Getter != null) OnMethodLooked(prop.Getter);
        }

        /// <summary>
        /// メソッド 通知 (プロパティや internal は通知されない)
        /// </summary>
        /// <param name="enumType"></param>
        protected override void OnMethodLooked(CLMethod method)
        {
            // あとでインデント量を調整するため、まずはテキストを覚えておく
            _oneClass.Add(
                new DocLine()
                {
                    Name = method.FuncDecl.OriginalFullName,
                    Brief = HSPHelpBuilder.TranslateComment(method.Document.OriginalBriefText)
                });
        }

        /// <summary>
        /// ファイルに出力するための最終文字列を生成する
        /// </summary>
        protected override string OnMakeOutoutFileText()
        {
            return _output.ToString();
        }
    }
}
