<#
.SYNOPSIS
    Push the mod to GitHub, wait for the CI build, download the compiled
    .geode from the nightly release, and install it into the Geometry Dash
    (Steam) Geode mods folder.

.PARAMETER SkipInstall
    Download the build but do not copy it into the GD mods folder.

.PARAMETER NoPush
    Skip the git commit/push step and just wait for the current HEAD build.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File deploy.ps1
#>

[CmdletBinding()]
param(
    [switch]$SkipInstall,
    [switch]$NoPush
)

$ErrorActionPreference = 'Stop'

$RepoDir   = Split-Path -Parent $MyInvocation.MyCommand.Path
$GdModsDir = 'C:\Program Files (x86)\Steam\steamapps\common\Geometry Dash\geode\mods'
$Headers   = @{ 'User-Agent' = 'geode-deploy-script' }
try {
    $cred = ("protocol=https`nhost=github.com" | git credential fill | Out-String)
    if ($cred -match 'password=(.+)') {
        $token = $matches[1].Trim()
        $Headers['Authorization'] = "Bearer $token"
    }
} catch {}

# ---- Read mod metadata from mod.json -------------------------------------
$mod = Get-Content -Raw (Join-Path $RepoDir 'mod.json') | ConvertFrom-Json
$modId     = $mod.id
$geodeName = "$modId.geode"
$repoUrl   = $mod.repository
$repoSlug  = ($repoUrl -replace '^https://github.com/', '').TrimEnd('/')
$apiBase   = "https://api.github.com/repos/$repoSlug"

Write-Host "==> Deploying $repoSlug ($modId)"

# ---- Commit & push -------------------------------------------------------
Set-Location $RepoDir

if (-not $NoPush) {
    $dirty = (git status --porcelain)
    if ($dirty) {
        git add -A
        git commit -m "deploy: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
    } else {
        Write-Host '==> Working tree clean, nothing to commit.'
    }
    Write-Host '==> Pushing to origin main...'
    git push origin main
    Write-Host '==> Pushed.'
}

$headSha = (git rev-parse HEAD).Trim()

# ---- Wait for the workflow run ------------------------------------------
Write-Host "==> Waiting for CI run for commit $headSha ..."
$run = $null
for ($i = 0; $i -lt 36; $i++) {
    $resp = Invoke-RestMethod -Uri "$apiBase/actions/runs?head_sha=$headSha&per_page=1" -Headers $Headers
    $run  = $resp.workflow_runs | Select-Object -First 1
    if ($run) { break }
    Start-Sleep -Seconds 5
}
if (-not $run) { throw "No workflow run found for commit $headSha" }

Write-Host ("==> Run {0} -> {1} | {2}" -f $run.id, $run.status, $run.html_url)
while ($run.status -ne 'completed') {
    Start-Sleep -Seconds 15
    $resp = Invoke-RestMethod -Uri "$apiBase/actions/runs?head_sha=$headSha&per_page=1" -Headers $Headers
    $run  = $resp.workflow_runs | Select-Object -First 1
    Write-Host ("   ... {0} ({1})" -f $run.status, $run.conclusion)
}

if ($run.conclusion -ne 'success') {
    throw "CI build FAILED (conclusion: $($run.conclusion)). See $($run.html_url)"
}
Write-Host '==> CI build succeeded.'

# ---- Download the compiled .geode ---------------------------------------
$dlUrl = "https://github.com/$repoSlug/releases/download/nightly/$geodeName"
$out    = Join-Path $RepoDir $geodeName
Write-Host "==> Downloading $dlUrl"
Invoke-WebRequest -Uri $dlUrl -OutFile $out -Headers $Headers

# ---- Install into Geometry Dash -----------------------------------------
if (-not $SkipInstall) {
    if (-not (Test-Path $GdModsDir)) {
        throw "GD mods folder not found: $GdModsDir"
    }
    $dest = Join-Path $GdModsDir $geodeName
    Copy-Item -Path $out -Destination $dest -Force
    Write-Host "==> Installed: $dest"

    $buildsDir = 'C:\Users\Joseph\Documents\GitHub\.geode-builds'
    if (Test-Path $buildsDir) {
        Copy-Item -Path $out -Destination (Join-Path $buildsDir $geodeName) -Force
        Write-Host "==> Backed up to: $(Join-Path $buildsDir $geodeName)"
    }
}

Write-Host '==> Done.'
