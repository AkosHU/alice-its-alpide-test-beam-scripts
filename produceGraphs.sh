#! /bin/bash

# December
  # Non irradiated
    # BB=0V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,2846, 3808, "2847 2849 2851 2853 2855 2857-2873 2880-2916 2923  2932-2933 2947-2949 2951 2953 2962-2970 2972-2973 2975-2977 2978-3670 3676 3678 3684 3686 3688-3703 3706 3710-3712 3714 3716 3718 3720 3722 3724 3726 3728 3730 3732-3737 3740-3741 3746 3748-3807");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,2846, 3811, "2847 2849 2851 2853 2855 2857-2873 2880-2916 2923 2947 2953 2962-2966 2968 2973 2975-3670 3688-3703 3706 3710-3712 3714 3716 3718 3720 3722 3732 3734-3737 3740-3741 3748-3807 3809-3810");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,2846, 3808, "2847 2849 2851 2853 2855 2857-2873 2880-2916 2923 2925-2953 2956 2962-2966 2968 2973 2975-2977 2978-3670 3688-3703 3706 3710-3712 3714 3716 3718 3720 3722 3724 3726 3728 3730 3732 3734-3737 3740-3741 3748-3807");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C


    # BB=-1V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,2978,3755,"2979 3035 3049-3747 3750");' >> tmp.C 
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,2978,3755,"2979 3049-3747 3750");' >> tmp.C 
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,2978,3755,"2979 3035 3049-3747 3750");' >> tmp.C 
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-2V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3049,3761,"3080 3092-3093 3107 3121-3755");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3049,3761,"3080 3092 3121-3755");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3049,3761,"3080 3083-3092 3107 3121-3755");' >> tmp.C 
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-3V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,2859,3767,"2859-2860 2862 2864 2866 2868 2870 2872-2879 2885 2889-2895 2897-3120 3145 3150 3164-3166 3178-3687 3704-3739 3742-3761");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,2859,3767,"2860 2862 2864 2866 2868 2870 2872-2879 2885 2889-2895 2897-3120 3145 3150 3164 3178-3687 3704-3739 3742-3761");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,2859,3767,"2860 2862 2864 2866-2868 2870-2879 2885 2888-2895 2897-3120 3145 3150 3164 3166 3178-3687 3704-3739 3742-3761");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-4V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3178,3838,"3184-3818");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3178,3838,"3184-3818");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3178,3838,"3184-3818");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-6V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,2890,3818 ,"2891 2894-2897 2899 2911-3185 3187 3197 3200-3768 3806-3814");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,2890,3818 ,"2891 2893-2897 2899 2901 2911-3185 3187 3189 3197 3200-3768 3806-3814");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,2909,3818 ,"2911-3185 3197 3199-3202 3204-3768 3806-3814");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

  # Irradiated
    # BB=0V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3218,3588,"3223-3226 3235 3238-3266 3291 3294 3296 3298 3305-3312 3314 3316 3318-3547 3549 3551 3553-3554 3356 3558-3578 3583");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3218,3588,"3223-3226 3235 3238-3266 3291 3298 3305-3307 3309-3310 3312 3314 3318-3546 3558-3578");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3227,3588,"3235 3238-3292 3298 3305-3307 3309-3310 3312 3314 3318-3546 3558-3578");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-1V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3318,3598,"3378-3588");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3318,3598,"3378-3588");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3318,3598,"3378-3588");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-2V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3378,3608,"3391 3429-3598");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3378,3608,"3391 3429-3598");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3378,3608,"3391 3429-3598");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-3V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3239,3619,"3251 3253 3255-3438 3475 3477 3479 3487-3558 3560 3575-3609");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3239,3619,"3251 3253 3255-3438 3475 3477 3487-3558 3560 3575-3609");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3239,3616,"3251 3253 3255-3438 3460-3461 3464-3558 3560 3575-3609");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-4V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3488,3630,"3489 3513 3522 3547-3575 3579-3620");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3488,3630,"3489 3513 3522 3547-3575 3579-3620");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3490,3630,"3513 3522 3547-3575 3579-3628");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-5V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3631,3638,"3634");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3631,3638,"3634");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3631,3638,"3634-3636");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

    # BB=-5V
      echo "{" > tmp.C
      echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",2,3255,3669,"3267-3638 3660");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",3,3255,3669,"3267-3638 3660");' >> tmp.C
      echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/",4,3255,3669,"3264-3638 3660");' >> tmp.C
      echo "}" >> tmp.C
      root -b -q tmp.C

  # Merging

    echo "{" > tmp.C
    echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-14_BB0V_Irr0_2846-3808_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-39_BB0V_Irr0_2846-3808_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W9-16_BB0V_Irr0_2846-3811_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-14_BB1V_Irr0_2978-3755_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-39_BB1V_Irr0_2978-3755_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W9-16_BB1V_Irr0_2978-3755_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-14_BB2V_Irr0_3049-3761_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-39_BB2V_Irr0_3049-3761_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W9-16_BB2V_Irr0_3049-3761_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-14_BB3V_Irr0_2859-3767_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-39_BB3V_Irr0_2859-3767_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W9-16_BB3V_Irr0_2859-3767_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-14_BB4V_Irr0_3178-3838_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-39_BB4V_Irr0_3178-3838_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W9-16_BB4V_Irr0_3178-3838_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-14_BB6V_Irr0_2909-3818_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-39_BB6V_Irr0_2890-3818_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W9-16_BB6V_Irr0_2890-3818_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C

    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-21_BB0V_Irr2_3218-3588_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-25_BB0V_Irr2_3227-3588_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W6-6_BB0V_Irr2_3218-3588_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-21_BB1V_Irr2_3318-3598_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-25_BB1V_Irr2_3318-3598_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W6-6_BB1V_Irr2_3318-3598_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C 
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-21_BB2V_Irr2_3378-3608_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-25_BB2V_Irr2_3378-3608_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W6-6_BB2V_Irr2_3378-3608_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W5-21_BB3V_Irr2_3239-3619_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W5-25_BB3V_Irr2_3239-3616_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-6_BB3V_Irr2_3239-3619_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-21_BB4V_Irr2_3488-3630_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-25_BB4V_Irr2_3490-3630_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W6-6_BB4V_Irr2_3488-3630_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W5-21_BB5V_Irr2_3631-3638_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W5-25_BB5V_Irr2_3631-3638_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/graphs_W6-6_BB5V_Irr2_3631-3638_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo 'mergeGraphs("/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-25_BB6V_Irr2_3255-3669_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W5-21_BB6V_Irr2_3255-3669_PR0.00um.root /mnt/data/scratch/analysis/pALPIDEfs/PS/Dec//graphs_W6-6_BB6V_Irr2_3255-3669_PR0.00um.root","/mnt/data/scratch/analysis/pALPIDEfs/PS/Dec/");' >> tmp.C
    echo "}" >> tmp.C
    root -b -q tmp.C
 # Sept-Nov
   # Non Irradiated
     # BB=0V W2-31
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1182,1953,"1190 1199-1211 1215-1248 1252-1274 1278-1284 1286-1862 1871-1886 1895-1910 1919-1938 1945-1947","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1200,1211,"1208-1211","6.03 5.53 5.33 5.68","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
     # BB=-3V W2-31
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1244,2223,"1245-1267 1273-2131 2134 2139-2142 2154 2163-2170 2183-2200","2.80 2.41 2.26 2.53","thr.dat","","","",-3);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
     # BB=-6V W2-31
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1252,1265,"1253","2.80 2.41 2.26 2.53","thr.dat","","","",-6);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
     # BB=0V FLEX
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,2236,2255,"2244 2246-2252 2254","","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
     # BB=0V W2-23
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1993,2020,"2001-2003 2009-2010 2016-2019","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
     # BB=0V W8-29
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,2023,2029,"","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
    # Irradiated with 0.25 x 10^13 
      # BB=0V W2-10
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1175,1181,"","2.80 2.41 2.26 2.53","thr.dat","","","",0)' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=0V W2-08
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1104,1110,"","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=-3V W2-08
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1111,2231,"1113 1119-2224 2229","2.80 2.41 2.26 2.53","thr.dat","","","",-3);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=-6V W2-08
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1119,1124,"","2.80 2.41 2.26 2.53","thr.dat","","","",-6);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=0V W2-09
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1160,1166,"","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1168,1174,"","6.03 5.53 5.33 5.68","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
    # Irradiated with 1 x 10^13 
      # BB=0V W2-15
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1153,1159,"","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=0V W2-12
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1097,1103,"","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=-3V W2-12
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1090,2200,"1096-2191","2.80 2.41 2.26 2.53","thr.dat","","","",-3);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=-6V W2-12
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1085,1089,"","2.80 2.41 2.26 2.53","thr.dat","","","",-6);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
    # Irradiated with 700 krad 
      # BB=0V W2-32
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1146,1151,"","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
    # Irradiated with 1 x 10^13 + 700 krad 
      # BB=0V W2-14
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1125,1130,"","2.80 2.41 2.26 2.53","thr.dat","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=-3V W2-14
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1131,1137,"","2.80 2.41 2.26 2.53","thr.dat","","","",-3);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=-6V W2-14
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/Sept-Nov/",3,1138,1145,"","2.80 2.41 2.26 2.53","thr.dat","","","",-6);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
  # July
      # BB=0V W2-23
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/July/",2,349,713,"352-353 356-358 360 366-390 394 398-399 403 413-416 429-456 458-462 486 493-661 664 668 670 676 679 682 687 700-701 703 707-709 712","","","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=0V W2-25
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/July/",3,349,713,"352-353 356-358 360 366-390 394 398-399 403 413-416 429-456 458-462 486 493-661 664 668 670 676 679 682 687 700-701 703 707-709 712","","","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=0V W2-27
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/July/",4,349,492,"352-353 356-358 360 366-390 394 398-399 403 413-416 429-456 458-462 486 ","","","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=0V W1-25
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/July/",4,662,713,"664 668 670 676 679 682 687 700-701 703 707-709 712","","","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
      # BB=-3V W1-25
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/July/",4,596,661,"597-598 601-603 610 628-630 635 645 650 658","","","","","",-3);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C
    # Irradiated 0.25 x 10^13
      # BB=0V W1-09
       echo "{" > tmp.C
       echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
       echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/PS/July/",2,915,929,"918 920 922-928","","","","","",0);' >> tmp.C
       echo "}" >> tmp.C
       root -b -q tmp.C

# SPS 
  # BB=0V
    echo "{" > tmp.C
    echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/SPS/Oct/",3,1652,1788,"1653-1659 1666 1675 1684 1693-1694 1703-1704 1713-1729 1738-1769 1778-1780","2.65 2.25 2.08 2.37","","","","",0);' >> tmp.C
    echo "}" >> tmp.C
    root -b -q tmp.C
  # BB=-3V
    echo "{" > tmp.C
    echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/SPS/Oct/",3,1740,1769,"1745 1755-1762","2.65 2.25 2.08 2.37","","","","",-3);' >> tmp.C
    echo "}" >> tmp.C
    root -b -q tmp.C
  # BB=-6V
    echo "{" > tmp.C
    echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/SPS/Oct/",3,1755,1760,"","2.65 2.25 2.08 2.37","","","","",-6);' >> tmp.C
    echo "}" >> tmp.C
    root -b -q tmp.C

# DESY
  # BB=0V
    echo "{" > tmp.C
    echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/DESY/Febr/",2,3900,4134,"3905-3909 3918-3919 3922-3945 3947-4008 4014-4018 4031-4103 4107 4115-4124");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/DESY/Febr/",3,3900,4134,"3905-3909 3918 3922-3945 3947-4008 4014-4018 4031-4103 4107 4115-4124");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/DESY/Febr/",4,3900,4134,"3905-3909 3918 3922-3945 3947-4008 4014-4018 4031-4103 4107 4115-4124");' >> tmp.C
    echo "}" >> tmp.C
    root -b -q tmp.C

  # BB=-3V
    echo "{" > tmp.C
    echo 'gROOT->LoadMacro("/home/analysis/ILCSOFT_DESY/palpidefs_scripts/plot_pALPIDEfs.C+");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/DESY/Febr/",2,3905,4144,"3910-3923 3945-3946 3952 3954-3955 3963 3979 4009-4013 4019-4030 4037 4050 4063 4104-4114 4125-4134");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/DESY/Febr/",3,3905,4144,"3910-3923 3944-3946 3950-3952 3954-3955 3963 3979 3987-4001 4009-4013 4019-4030 4037 4050 4056 4063 4104-4114 4125-4134");' >> tmp.C
    echo 'WriteGraph("/mnt/data/scratch/analysis/pALPIDEfs/DESY/Febr/",4,3905,4144,"3910-3923 3944-3946 3951-3952 3954-3955 3963 3979 3987-4001 4009-4013 4019-4030 4037 4050 4063 4104-4114 4125-4134");' >> tmp.C
    echo "}" >> tmp.C
    root -b -q tmp.C


rm tmp.C
