using System;
using System.IO;
using LuminoBuild;
using System.Text;
using System.IO.Compression;

namespace LuminoBuild.Tasks
{
    class SetupDependencies : BuildTask
    {
        public override string CommandName { get { return "SetupDependencies"; } }

        public override string Description { get { return "Setup dependencies."; } }

        public override void Build(Builder builder)
        {
            var zipPath = builder.LuminoRootDir + "External/LuminoDependencies.zip";
            if (File.Exists(zipPath)) return;   // もう完了している

            var extractDir = builder.LuminoRootDir + "External";

            Logger.WriteLine("Downloading dependencies...");
            var wc = new System.Net.WebClient();
            wc.DownloadFile(
                "https://github.com/lriki/LuminoDependencies/archive/v2.zip",
                zipPath);
            wc.Dispose();

            Logger.WriteLine("Extracting...");
            ZipFile.ExtractToDirectory(zipPath, extractDir);

            Directory.Move(extractDir + "/LuminoDependencies-2", extractDir + "/LuminoDependencies");

            // TODO: 含まれている zip は全部自動展開でいいかも？
            string toolsDir = extractDir + "/LuminoDependencies/Tools/";
            ZipFile.ExtractToDirectory(toolsDir + "wix311-binaries.zip", toolsDir + "wix311-binaries");
        }
    }
}
