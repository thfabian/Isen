function namelist()                                           

% Global variables 
% ------------------------ 
global g cp r r_v rdcp cpdr pref th00 z00 prs00 exn00 u00 bv00    ...
 topomx topowd topotim nab diffabs irelax idbg iprtcfl imoist ...
imoist_diff imicrophys idthdt iout xl nx dx thl nz nb dt time   ...
iiniout diff run_name ishear k_shl k_sht u00_sh ...
vt_mult autoconv_th autoconv_mult iern itime sediment_on


% --------------------------------------------------------
% --------------------- USER NAMELIST --------------------
% --------------------------------------------------------

% Domain size
% -----------------------------------
xl      = 500000.;      % domain size[m]
nx      = 100;          % horizontal resolution
dx      = xl/nx;
thl     = 60.;          % domain depth [K]
nz      = 60;           % vertical resolution
dt      = 10;           % time step [s]
time    = 10;      % integration time [s]
diff    = 0.02;         % (horizontal) diffusion coefficient

% Topography
% -----------------------------------
topomx  = 500;         % mountain height [m]
topowd  = 50000;       % mountain half width [m]
topotim = 1800;        % mountain growth time [s]

% Initial atmosphere 
% -----------------------------------
u00     = 15;          % initial velocity [m/s]
bv00    = 0.01;        % Brunt-Vaisalla frequency [1/s]
th00    = 300.;	       % potential temperature at surface

ishear  = 0;           % wind shear simulation (0=no shear,1=shear)
k_shl   = 5;	       % bottom level of wind shear layer    (ishear=1)      
k_sht   = 8;	       % top level of wind shear layer       (ishear=1)
u00_sh  = 10.; 	       % initial velocity below shear layer [m/s] (ishear=1)
                       % u00 is speed above shear layer [m/s] 

% Boundaries
% -----------------------------------
nab     = 0;            % number of grid points in absorber
diffabs = 1.;           % maximum value of absorber
irelax  = 0;            % lateral boundaries (0=periodic, 1=relax)
nb      = 2;            % number of boundary points on each side

% Output control
% -----------------------------------
iout    = 360 ;         % write every iout-th time-step into the output file
iiniout = 1;            % write initial fields (0=no,1=yes)
run_name = 'logfile';   % simulation name

% Print options
% -----------------------------------
idbg    = 0;           % print debugging text (0=not print, 1=print)
iprtcfl = 1;           % print Courant number (0=not print, 1=print)
itime   = 1;           % print computation time (0=not print, 1=print)

% Physics: Moisture
% -----------------------------------
imoist         = 0;    % include moisture             (0=dry, 1=moist)
imoist_diff    = 0;    % apply diffusion to qv,qc,qr  (0=off, 1=on)
imicrophys     = 0;    % include microphysics         (0=off, 1=kessler, 2=two moment)
idthdt         = 0;    % couple physics to dynamics   (0=off, 1=on)
iern           = 0;    % evaporation of rain droplets (0=off, 1=on)

% Options for Kessler scheme
% -----------------------------------
vt_mult        = 1.;     % multiplication factor for terminal fall velocity
autoconv_th    = 0.0001; % critical cloud water mixing ratio for the onset 
                         % of autoconversion [kg/kg]
autoconv_mult  = 1.;     % multiplication factor for autoconversion 
sediment_on    = 1;      % switch to turn on / off sedimentation

% --------------------------------------------------------
% --------------------------------------------------------
% --------------------------------------------------------

% Physical constants 
% ------------------------
g       = 9.81;                 % gravity
cp      = 1004.;                % specific heat of air at constant pressure
r       = 287.;                 % gas constant of air [J/kgK]
r_v     = 461.;                 % gas constant of vapor [J/kgK]
rdcp    = r/cp;                 % short cut for R/Cp
cpdr    = cp/r;                 % short cut for Cp/R
pref    = 100*1000.;            % reference pressure in SI units (Pa, not hPa!)
z00     = 0.;                   % surface height
prs00   = pref;                 % upstream surface pressure ( = ref. pressure )
exn00   = cp*(prs00/pref)^rdcp;

