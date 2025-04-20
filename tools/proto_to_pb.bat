@echo off
:: proto_to_pb.bat - 自动将.proto文件编译为C++代码
:: 用法: proto_to_pb.bat [proto_dir] [output_dir] [protoc_path]

setlocal enabledelayedexpansion

:: ============= 参数配置 =============
:: 默认值
set "proto_dir=%~1"
if "%proto_dir%"=="" set "proto_dir=%cd%"

set "output_dir=%~2"
if "%output_dir%"=="" set "output_dir=%proto_dir%\generated"

set "protoc_path=%~3"
if "%protoc_path%"=="" set "protoc_path=protoc"

:: ============= 检查protoc =============
%protoc_path% --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 找不到protoc或无法执行
    echo 请确保protoc在PATH中，或通过第三个参数指定完整路径
    echo 例如: proto_to_pb.bat . generated "C:\protobuf\bin\protoc.exe"
    pause
    exit /b 1
)

:: ============= 准备输出目录 =============
if not exist "%output_dir%" (
    echo 创建输出目录: %output_dir%
    mkdir "%output_dir%"
)

:: ============= 处理.proto文件 =============
set /a success_count=0
set /a fail_count=0

echo.
echo 正在从 %proto_dir% 生成Protobuf代码...
echo 输出目录: %output_dir%
echo.

for /r "%proto_dir%" %%f in (*.proto) do (
    set "rel_path=%%~dpf"
    set "rel_path=!rel_path:%proto_dir%=!"
    
    :: 为每个.proto文件创建对应的输出子目录
    if not "!rel_path!"=="" (
        set "target_dir=%output_dir%!rel_path!"
        if not exist "!target_dir!" mkdir "!target_dir!"
    ) else (
        set "target_dir=%output_dir%"
    )
    
    echo 正在处理: %%~nxf
    echo 输出到: !target_dir!
    
    :: 执行protoc命令
    %protoc_path% --proto_path="%proto_dir%" --cpp_out="!target_dir!" "%%f"
    
    if %errorlevel% equ 0 (
        set /a success_count+=1
        echo [成功]
    ) else (
        set /a fail_count+=1
        echo [失败]
    )
    echo.
)

:: ============= 结果汇总 =============
echo ============= 生成结果 =============
echo 成功: %success_count% 个文件
echo 失败: %fail_count% 个文件
echo 输出目录: %output_dir%
echo ===================================

:: 打开输出目录
set /p choice=是否要打开输出目录? [Y/N] 
if /i "%choice%"=="y" explorer "%output_dir%"

endlocal
pause