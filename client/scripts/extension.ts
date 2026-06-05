import * as path from 'path';
import * as vscode from 'vscode';
import {
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
    TransportKind
} from 'vscode-languageclient/node';

let client: LanguageClient;

export function activate(context: vscode.ExtensionContext) {
    console.log('Extension activated');

    // Path to your compiled C++ LSP server executable
    const serverExe = context.asAbsolutePath(
        path.join('server', 'out', 'bin', 'windows', 'rqmt.exe') // adjust name/path
    );

    console.log(path.join('server', 'out', 'bin', 'windows', 'rqmt.exe'));

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
