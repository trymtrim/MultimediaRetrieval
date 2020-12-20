using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using UnityEngine;

public static class Backend
{
    private const bool Debug = false;

    private enum Command
    {
        Query,
        Store,
    }

    private class Args
    {
        private readonly Command _command;
        private string InputFile { get; }
        private string Database { get; }
        private bool _append;

        private bool Append
        {
            set
            {
                if (value && _overwrite)
                {
                    _overwrite = false;
                }

                _append = value;
            }
        }

        private bool _overwrite;

        private bool Overwrite
        {
            set
            {
                if (value && _append)
                {
                    _append = false;
                }

                _overwrite = value;
            }
        }

        // ReSharper disable once MemberHidesStaticFromOuterClass
        private bool Debug { get; }

        internal Args(
            Command command,
            string inputFile = null,
            string database = null,
            bool append = false,
            bool overwrite = false,
            bool debug = false
        )
        {
            _command = command;

            InputFile = inputFile;
            Database = database;

            // Switched when compared to args, so that append has priority.
            Overwrite = overwrite;
            Append = append;

            Debug = debug;
        }

        public override string ToString()
        {
            var command = $"--{_command.ToString().ToLower()}";
            var inputFile = string.IsNullOrEmpty(InputFile) ? "" : $" {InputFile}";
            var database = string.IsNullOrEmpty(Database) ? "" : $" --database {Database}";
            var append = !_append ? "" : " --append";
            var overwrite = !_overwrite ? "" : " --overwrite";
            var debug = !Debug ? "" : " --debug";
            return $"{command}{inputFile}{database}{append}{overwrite}{debug}";
        }
    }

    public class Result
    {
        internal enum ReturnType
        {
            Ok,
            NotFound,
            Error,
            None,
        }

        // ReSharper disable once CollectionNeverQueried.Global
        internal List<string> OutputData { get; }

        // ReSharper disable once CollectionNeverQueried.Global
        internal List<string> ErrorData { get; }

        // ReSharper disable once CollectionNeverQueried.Global
        // ReSharper disable once UnusedAutoPropertyAccessor.Global
        internal ReturnType Type { get; set; }

        internal Result()
        {
            OutputData = new List<string>();
            ErrorData = new List<string>();
            Type = ReturnType.None;
        }
    }

    private static string ExecutablePath()
    {
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
            case RuntimePlatform.OSXPlayer:
                return Path.GetFullPath("Backend/macOS");
            case RuntimePlatform.WindowsPlayer:
            case RuntimePlatform.WindowsEditor:
                return Path.GetFullPath("Backend/windows.exe");
            case RuntimePlatform.LinuxPlayer:
            case RuntimePlatform.LinuxEditor:
                throw new ArgumentOutOfRangeException();
            default:
                throw new ArgumentOutOfRangeException();
        }
    }

    private static Result RunProcess(Args args)
    {
        var process = new Process
        {
            StartInfo =
            {
                FileName = ExecutablePath(),
                Arguments = args.ToString(),
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true
            }
        };
        var result = new Result();

        process.OutputDataReceived += (s, e) => { result.OutputData.Add(e.Data); };
        process.ErrorDataReceived += (s, e) => { result.ErrorData.Add(e.Data); };

        process.Start();
        process.BeginOutputReadLine();
        process.BeginErrorReadLine();
        process.WaitForExit();

        switch (process.ExitCode)
        {
            case 0:
                result.Type = Result.ReturnType.Ok;
                break;
            case 6:
                result.Type = Result.ReturnType.NotFound;
                break;
            default:
                result.Type = Result.ReturnType.Error;
                break;
        }

        return result;
    }

    // ReSharper disable once UnusedMember.Global
    public static Result Query(string inputFile, string database = null, bool debug = Debug)
    {
        // ReSharper disable once ConvertIfStatementToNullCoalescingAssignment
        if (database == null)
            database = Util.GetDatabasePath();
        return RunProcess(new Args(Command.Query, inputFile, database, debug: debug));
    }

    // ReSharper disable once UnusedMember.Global
    public static Result Store(string inputFile, string database = null, bool debug = Debug)
    {
        // ReSharper disable once ConvertIfStatementToNullCoalescingAssignment
        if (database == null)
            database = Util.GetDatabasePath();
        return RunProcess(new Args(Command.Store, inputFile, database, true, debug: debug));
    }
}