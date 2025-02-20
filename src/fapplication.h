/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2019 Fritzing

Fritzing is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Fritzing is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fritzing.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************/

#ifndef FAPPLICATION_H
#define FAPPLICATION_H

#include <QApplication>
#include <QTranslator>
#include <QPixmap>
#include <QFileDialog>
#include <QPointer>
#include <QWidget>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QThread>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "referencemodel/referencemodel.h"

class FileProgressDialog;

class FServer : public QTcpServer
{
	Q_OBJECT

public:
	FServer(QObject *parent = 0);

Q_SIGNALS:
	void newConnection(qintptr socketDescriptor);

protected:
	void incomingConnection(qintptr socketDescriptor);
};

class FServerThread : public QThread
{
	Q_OBJECT

public:
	FServerThread(qintptr socketDescriptor, QObject *parent);

	void run();
	void setDone();

Q_SIGNALS:
	void error(QTcpSocket::SocketError socketError);
	void doCommand(const QString & command, const QString & params, QString & result, int & status);

protected:
	void writeResponse(QTcpSocket *, int code, const QString & codeString, const QString & mimeType, const QString & message);

protected:
	int m_socketDescriptor = 0;
	bool m_done = false;

protected:
	static QMutex m_busy;

};

////////////////////////////////////////////////////

class RegenerateDatabaseThread : public QThread
{
	Q_OBJECT
public:
	RegenerateDatabaseThread(const QString & dbFileName, QDialog *progressDialog, ReferenceModel *referenceModel);
	const QString error() const;
	QDialog * progressDialog() const;
	ReferenceModel * referenceModel() const;

protected:
	void run() Q_DECL_OVERRIDE;

protected:
	QString m_dbFileName;
	QString m_error;
	QDialog * m_progressDialog = nullptr;
	ReferenceModel * m_referenceModel = nullptr;
};

////////////////////////////////////////////////////


class FApplication : public QApplication
{
	Q_OBJECT

public:
	FApplication(int & argc, char ** argv);
	~FApplication(void);

public:
	int init();
	int startup();
	int serviceStartup();
	void finish();
	bool loadReferenceModel(const QString & databaseName, bool fullLoad);
	bool loadReferenceModel(const QString & databaseName, bool fullLoad, ReferenceModel * referenceModel);
	void registerFonts();
	class MainWindow * openWindowForService(bool lockFiles, int initialTab);
	bool runAsService();

public:
	static bool spaceBarIsPressed();

Q_SIGNALS:
	void spaceBarIsPressedSignal(bool);

public Q_SLOTS:
	void preferences();
	void preferencesAfter();
	void checkForUpdates();
	void checkForUpdates(bool atUserRequest);
	void enableCheckUpdates(bool enabled);
	void createUserDataStoreFolderStructures();
	void changeActivation(bool activate, QWidget * originator);
	void updateActivation();
	void topLevelWidgetDestroyed(QObject *);
	void closeAllWindows2();
	void loadedPart(int loaded, int total);
	void externalProcessSlot(QString & name, QString & path, QStringList & args);
	void gotOrderFab(QNetworkReply *);
	void newConnection(qintptr socketDescriptor);
	void doCommand(const QString & command, const QString & params, QString & result, int & status);
	void regeneratePartsDatabase();
	void regenerateDatabaseFinished();
	void installNewParts();


protected:
	bool eventFilter(QObject *obj, QEvent *event);
	bool event(QEvent *event);
	bool findTranslator(const QString & translationsPath);
	void loadNew(QString path);
	void loadOne(class MainWindow *, QString path, int loaded);
	void initSplash(class FSplashScreen & splash);
	void registerFont(const QString &fontFile, bool reallyRegister);
	void clearModels();
	bool notify(QObject *receiver, QEvent *e);
	void initService();
	void runDRCService();
	void runGedaService();
	void runDatabaseService();
	void runKicadFootprintService();
	void runKicadSchematicService();
	void runGerberService();
	void runGerberServiceAux();
	void runExportAllService();
	void runExportAllServiceAux();
	void runSvgService();
	void runSvgServiceAux();
	void runExampleService();
	void runExampleService(QDir &);
	QList<class MainWindow *> recoverBackups();
	QList<MainWindow *> loadLastOpenSketch();
	void doLoadPrevious(MainWindow *);
	void loadSomething(const QString & previousVersion);
	void initFilesToLoad();
	void initBackups();
	void cleanupBackups();
	void updatePrefs(class PrefsDialog & prefsDialog);
	QList<MainWindow *> orderedTopLevelMainWindows();
	void cleanFzzs();
	void initServer();
	void regeneratePartsDatabaseAux(QDialog * progressDialog);


	enum class ServiceType {
		GerberService = 1,
		GedaService,
		KicadSchematicService,
		KicadFootprintService,
		ExampleService,
		DatabaseService,
		SvgService,
		PortService,
		DRCService,
		ExportAllService,
		NoService
	};

protected:
	bool m_spaceBarIsPressed = false;
	bool m_mousePressed = false;
	QTranslator m_translator;
	ReferenceModel * m_referenceModel = nullptr;
	bool m_started = false;
	QStringList m_filesToLoad;
	QString m_libPath;
	QString m_translationPath;
	class UpdateDialog * m_updateDialog = nullptr;
	QTimer m_activationTimer;
	QPointer<class FritzingWindow> m_lastTopmostWindow;
	QList<QWidget *> m_orderedTopLevelWidgets;
	QStringList m_arguments;
	QStringList m_externalProcessArgs;
	QString m_externalProcessName;
	QString m_externalProcessPath;
	ServiceType m_serviceType = ServiceType::NoService;
	int m_progressIndex = 0;
	class FSplashScreen * m_splash = nullptr;
	QString m_outputFolder;
	QString m_portRootFolder;
	QString m_panelFilename;
	QHash<QString, struct LockedFile *> m_lockedFiles;
	int m_portNumber = 0;
	FServer * m_fServer = nullptr;
	QString m_buildType;
};


enum FInitResult {
	FInitResultNormal,
	FInitResultHelp,
	FInitResultVersion
};

#endif
