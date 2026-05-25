import {randomUUID} from 'node:crypto';

import type {LaunchGateReasonCode} from '../../../lib/contracts/launchGateReasonCodes.ts';

export type RemediationTicket = {
  remediationTicketId: string; reasonCode: LaunchGateReasonCode;
  webFlowUrl: string;
  requiredActions: string[];
  status: 'pending' | 'in-progress' | 'completed' | 'expired';
  createdAt: string;
  updatedAt: string;
  retryAllowedAfter: string;
  completedAt?: string;
};

export interface RemediationTicketService {
  create(reasonCode: LaunchGateReasonCode): RemediationTicket;
  get(remediationTicketId: string): RemediationTicket|null;
  complete(remediationTicketId: string): RemediationTicket|null;
}

class InMemoryRemediationTicketService implements RemediationTicketService {
  private readonly tickets = new Map<string, RemediationTicket>();

  create(reasonCode: LaunchGateReasonCode): RemediationTicket {
    const remediationTicketId = randomUUID();
    const createdAt = new Date().toISOString();
    const retryAllowedAfter = new Date(Date.now() + 10_000).toISOString();
    const ticket: RemediationTicket = {
      remediationTicketId,
      reasonCode,
      webFlowUrl: 'https://banana.engineer/account/link',
      requiredActions: [
        'Sign in to Banana account portal',
        'Link Steam identity to production account',
        'Retry launch verification',
      ],
      status: 'pending',
      createdAt,
      updatedAt: createdAt,
      retryAllowedAfter,
    };

    this.tickets.set(remediationTicketId, ticket);
    return ticket;
  }

  get(remediationTicketId: string): RemediationTicket|null {
    return this.tickets.get(remediationTicketId) ?? null;
  }

  complete(remediationTicketId: string): RemediationTicket|null {
    const existing = this.tickets.get(remediationTicketId);
    if (!existing) {
      return null;
    }

    const completed: RemediationTicket = {
      ...existing,
      status: 'completed',
      updatedAt: new Date().toISOString(),
      completedAt: new Date().toISOString(),
    };
    this.tickets.set(remediationTicketId, completed);
    return completed;
  }

  reset(): void {
    this.tickets.clear();
  }
}

const remediationTicketService = new InMemoryRemediationTicketService();

export function createRemediationTicketService(): RemediationTicketService {
  return remediationTicketService;
}

export function resetRemediationTicketServiceForTests(): void {
  remediationTicketService.reset();
}
