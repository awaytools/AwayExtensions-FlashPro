How to package the extension:
	

	Optional step: create certificate (there is already a certificate present at \CEP_cmd_tools\AwayCertificate.p12)
	
		Use the \CEP_cmd_tools\CEP-Resources-master\CEP-Resources-master\ZXPSignCMD\3.0.30\win64\ZXPSignCmd.exe to generate a certificate.

		The Certificate should be created with the option -validitiyDays set to 9999
		
	Use Eclipse to package the extension into a zxp file:
		
		You need eclipse with the adobe extension-build plugin.
		Adjust the manifest.xml for a new version number, than export as adobe extension (brings you to a option page that allows to chosse the certificate)
		
	Use the Extensionmanager tool (\CEP_cmd_tools\ExtensionManager) to install the zxp on win or mac
	
	

