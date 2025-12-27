using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using Microsoft.Win32;

namespace GuiApp
{
    public partial class MainWindow : Window
    {
        private Process engineProc;
        private readonly List<string> history = new List<string>();
        private int historyIndex = -1;

        public MainWindow()
        {
            InitializeComponent();
            OutputBox.Text = "";
            RegisterKeyboardShortcuts();
        }

        private void RegisterKeyboardShortcuts()
        {
            this.InputBindings.Add(new KeyBinding(
                new RelayCommand(() => StartButton_Click(null, null)),
                new KeyGesture(Key.F5)));

            this.InputBindings.Add(new KeyBinding(
                new RelayCommand(() => StopButton_Click(null, null)),
                new KeyGesture(Key.F5, ModifierKeys.Shift)));

            this.InputBindings.Add(new KeyBinding(
                new RelayCommand(() => SaveOutputButton_Click(null, null)),
                new KeyGesture(Key.S, ModifierKeys.Control)));

            this.InputBindings.Add(new KeyBinding(
                new RelayCommand(() => ClearInputButton_Click(null, null)),
                new KeyGesture(Key.L, ModifierKeys.Control)));
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            if (AutoStartCheck.IsChecked == true)
            {
                Dispatcher.BeginInvoke(new Action(() => StartEngineIfPossible()));
            }
        }

        private void StartEngineIfPossible()
        {
            if (engineProc == null || engineProc.HasExited)
            {
                StartButton_Click(this, null);
            }
        }

        private void AppendOutput(string text)
        {
            Dispatcher.Invoke(() =>
            {
                if (!string.IsNullOrEmpty(text))
                {
                    OutputBox.AppendText(text + Environment.NewLine);
                    OutputBox.ScrollToEnd();
                }
            });
        }

        private string TryFindEngineExe()
        {
            string baseDir = AppDomain.CurrentDomain.BaseDirectory;
            string[] quickCandidates =
            {
                Path.Combine(baseDir, "Db engine.exe"),
                Path.Combine(baseDir, "Db-engine.exe"),
            };
            foreach (var c in quickCandidates)
                if (File.Exists(c)) return Path.GetFullPath(c);

            DirectoryInfo dir = new DirectoryInfo(baseDir);
            for (int up = 0; up < 5 && dir != null; up++)
            {
                try
                {
                    string[] subCandidates =
                    {
                        Path.Combine(dir.FullName, "Db engine", "Debug", "Db engine.exe"),
                        Path.Combine(dir.FullName, "Db engine", "Release", "Db engine.exe"),
                        Path.Combine(dir.FullName, "Debug", "Db engine.exe"),
                        Path.Combine(dir.FullName, "Release", "Db engine.exe")
                    };
                    foreach (var sc in subCandidates)
                        if (File.Exists(sc)) return Path.GetFullPath(sc);

                    var found = Directory.EnumerateFiles(dir.FullName, "Db engine.exe", SearchOption.AllDirectories).FirstOrDefault();
                    if (!string.IsNullOrEmpty(found)) return Path.GetFullPath(found);
                }
                catch
                {
                }

                dir = dir.Parent;
            }

            return null;
        }

        private void UpdateUiRunning(bool running)
        {
            StartButton.IsEnabled = !running;
            StopButton.IsEnabled = running;
            SendButton.IsEnabled = running;

            if (running)
            {
                StatusBlock.Text = "Engine Running";
                StatusIndicator.Fill = new SolidColorBrush(Color.FromRgb(16, 124, 16)); // Green
            }
            else
            {
                StatusBlock.Text = "Engine Stopped";
                StatusIndicator.Fill = new SolidColorBrush(Color.FromRgb(110, 110, 110)); // Gray
            }
        }

        private void StartButton_Click(object sender, RoutedEventArgs e)
        {
            if (engineProc != null && !engineProc.HasExited)
                return;

            string exePath = TryFindEngineExe();

            if (string.IsNullOrEmpty(exePath) || !File.Exists(exePath))
            {
                AppendOutput("❌ Error: engine executable not found");
                AppendOutput("💡 Hints:");
                AppendOutput("   - Build the native 'Db engine' project first.");
                AppendOutput("   - Ensure the C++ project output is accessible.");
                AppendOutput("   - Check Solution > Project Dependencies...");
                MessageBox.Show(
                    "Database engine executable not found.\n\nPlease build the 'Db engine' C++ project first.",
                    "Engine Not Found",
                    MessageBoxButton.OK,
                    MessageBoxImage.Warning);
                return;
            }

            StatusPathBlock.Text = Path.GetFileName(exePath);
            StartEngine(exePath);
        }

        private void StartEngine(string exePath)
        {
            var psi = new ProcessStartInfo
            {
                FileName = exePath,
                UseShellExecute = false,
                RedirectStandardInput = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true,
                StandardOutputEncoding = Encoding.UTF8,
                StandardErrorEncoding = Encoding.UTF8,
                WorkingDirectory = Path.GetDirectoryName(exePath)
            };

            engineProc = new Process { StartInfo = psi, EnableRaisingEvents = true };
            engineProc.OutputDataReceived += (s, ea) => { if (ea.Data != null) AppendOutput(ea.Data); };
            engineProc.ErrorDataReceived += (s, ea) => { if (ea.Data != null) AppendOutput("⚠ [ERR] " + ea.Data); };
            engineProc.Exited += (s, ea) =>
            {
                AppendOutput("🛑 Engine exited.");
                Dispatcher.Invoke(() => UpdateUiRunning(false));
            };

            try
            {
                engineProc.Start();
                engineProc.BeginOutputReadLine();
                engineProc.BeginErrorReadLine();
                AppendOutput("✅ Engine started successfully.");
                AppendOutput("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
                UpdateUiRunning(true);
                InputBox.Focus();
            }
            catch (Exception ex)
            {
                AppendOutput("❌ Failed to start engine: " + ex.Message);
                engineProc = null;
                UpdateUiRunning(false);
            }
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            StopEngine();
        }

        private void StopEngine()
        {
            if (engineProc == null || engineProc.HasExited)
            {
                AppendOutput("⚠ Engine is not running.");
                engineProc = null;
                UpdateUiRunning(false);
                return;
            }

            try
            {
                try
                {
                    engineProc.StandardInput.WriteLine("EXIT");
                    engineProc.StandardInput.Flush();
                }
                catch { }

                if (!engineProc.WaitForExit(1500))
                {
                    try { engineProc.Kill(); } catch { }
                }

                AppendOutput("🛑 Engine stopped.");
            }
            catch (Exception ex)
            {
                AppendOutput("❌ Error stopping engine: " + ex.Message);
            }
            finally
            {
                engineProc?.Dispose();
                engineProc = null;
                UpdateUiRunning(false);
            }
        }

        private void SendButton_Click(object sender, RoutedEventArgs e)
        {
            SendInput();
        }

        private void InputBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter && (Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control)
            {
                SendInput();
                e.Handled = true;
                return;
            }

            if (e.Key == Key.Up)
            {
                if (history.Count > 0)
                {
                    if (historyIndex == -1) historyIndex = history.Count;
                    historyIndex = Math.Max(0, historyIndex - 1);
                    InputBox.Text = history[historyIndex];
                    InputBox.CaretIndex = InputBox.Text.Length;
                    e.Handled = true;
                }
            }
            else if (e.Key == Key.Down)
            {
                if (history.Count > 0 && historyIndex != -1)
                {
                    historyIndex = Math.Min(history.Count - 1, historyIndex + 1);
                    InputBox.Text = historyIndex < history.Count ? history[historyIndex] : "";
                    InputBox.CaretIndex = InputBox.Text.Length;
                    e.Handled = true;
                }
            }
        }

        private void SendInput()
        {
            var cmd = InputBox.Text.Trim();
            if (string.IsNullOrWhiteSpace(cmd)) return;

            if (engineProc == null || engineProc.HasExited)
            {
                AppendOutput("⚠ Engine is not running. Start it first.");
                return;
            }

            try
            {
                engineProc.StandardInput.WriteLine(cmd);
                engineProc.StandardInput.Flush();
                AppendOutput($"⚡ > {cmd}");
                history.Add(cmd);
                historyIndex = -1;
                InputBox.Clear();
            }
            catch (Exception ex)
            {
                AppendOutput("❌ Failed to send command: " + ex.Message);
            }
        }

        private void ClearButton_Click(object sender, RoutedEventArgs e)
        {
            OutputBox.Clear();
            AppendOutput("Console output cleared.");
        }

        private void ClearInputButton_Click(object sender, RoutedEventArgs e)
        {
            InputBox.Clear();
            InputBox.Focus();
        }

        private void SaveOutputButton_Click(object sender, RoutedEventArgs e)
        {
            var dlg = new SaveFileDialog
            {
                Filter = "Text files (*.txt)|*.txt|Log files (*.log)|*.log|All files (*.*)|*.*",
                FileName = $"engine-output-{DateTime.Now:yyyyMMdd-HHmmss}.txt",
                Title = "Save Engine Output"
            };
            if (dlg.ShowDialog() == true)
            {
                try
                {
                    File.WriteAllText(dlg.FileName, OutputBox.Text, Encoding.UTF8);
                    AppendOutput($"💾 Output saved to {Path.GetFileName(dlg.FileName)}");
                }
                catch (Exception ex)
                {
                    AppendOutput("❌ Failed to save file: " + ex.Message);
                    MessageBox.Show($"Failed to save file:\n{ex.Message}", "Save Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            StopEngine();
        }
    }

    public class RelayCommand : ICommand
    {
        private readonly Action _execute;

        public RelayCommand(Action execute)
        {
            _execute = execute;
        }

        public bool CanExecute(object parameter) => true;
        public void Execute(object parameter) => _execute();
        public event EventHandler CanExecuteChanged;
    }
}