from CRABClient.UserUtilities import config
config = config()

config.section_('General')
config.General.requestName = 'Three-particleCorrWithEffCorr'
config.General.workArea = 'PbPb'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'trihadronanalyzerPbPb_cfg.py'
config.JobType.outputFiles = ['TriHadronFirstAttempt.root']
config.JobType.inputFiles = ['CorrectionFactors_PbPb.root']

config.section_('Data')
config.Data.inputDataset = '/HIMinBiasUPC/HIRun2011-14Mar2014-v2/RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10
config.Data.lumiMask = 'Cert_181530-183126_HI7TeV_PromptReco_Collisions11_JSON.txt'
config.Data.publication = False
#config.Data.ignoreLocality = True
config.Data.allowNonValidInputDataset = True
#config.Data.runRange = '193093-193999' # '193093-194075'
#config.Data.outLFN = '/store/user/<subdir>' # or '/store/group/<subdir>'
#config.Data.publishDataName = 'CRAB3_tutorial_Data_analysis_test5'

config.section_('Site')
config.Site.storageSite = 'T2_US_Vanderbilt'