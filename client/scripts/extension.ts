import * as path from 'path';
import * as vscode from 'vscode';
import {
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
    TransportKind
} from 'vscode-languageclient/node';

// client.ts
function getOS(): string {
  switch (process.platform) {
    case 'win32': return 'windows';
    case 'linux': return 'linux';
    case 'darwin': return 'macos';
    default: return 'Unknown';
  }
}

let client: LanguageClient;

export function activate(context: vscode.ExtensionContext) {
    console.log('Extension activated');

    // Path to your compiled C++ LSP server executable
    const pathToExe = path.join('server', 'out', 'bin', getOS(), process.platform === 'win32' ? 'rqmt.exe' : 'rqmt');

    const serverExe = context.asAbsolutePath(pathToExe);

    console.log(pathToExe);

    const serverOptions: ServerOptions = {
        run: {
            command: serverExe,
            transport: TransportKind.stdio
        },
        debug: {
            command: serverExe,
            transport: TransportKind.stdio
        }
    };

    const clientOptions: LanguageClientOptions = {
        documentSelector: [{ scheme: 'file', language: '*' }], // change language

        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher('**//*.rqmt')
        },

        outputChannel: vscode.window.createOutputChannel('RQMT-LSP-Client')
    };

    
    client = new LanguageClient(
      'RQMT_LSP_client',
      'RQMT LSP Client',
      serverOptions,
      clientOptions
    );
    
    // Log lifecycle events
    client.onDidChangeState((event) => {
        console.log('LSP state changed:', event);
    });

    client.start();
}

export function deactivate(): Thenable<void> | undefined {
    console.log('Extension deactivated');
    if (!client) {
        return undefined;
    }
    return client.stop();
}
