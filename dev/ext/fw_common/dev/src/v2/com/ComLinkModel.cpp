/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : GenComLink.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComLinkModel.h>
#include <v2/com/ComLinkSerial.h>
#include <v2/com/ComLinkTcpCli.h>

// -- Qt include
#include <QSerialPortInfo>

//ComLinkManager * ComLinkManager::m_pMyInstance = nullptr;

ComLinkModel::ComLinkModel(ComLinkManager * parent, QList<GenComLink *> * p_plLink) : QAbstractListModel(parent), Loggable("ComLinkModel")
{
    m_pComLinkManager = parent;
    m_plLink = p_plLink;
}

int ComLinkModel::rowCount(const QModelIndex&) const
{
    return m_plLink->size();
}

int ComLinkModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant ComLinkModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (index.row() < 0 || index.row() >= m_plLink->size() )
           return QVariant();
    GenComLink* l = m_plLink->at(index.row());

    if (role == ROLE_LINK_TYPE ) {
        ret = l->getType();
    } else if (role == ROLE_LINK_NAME ) {
        ret = l->getName();
    } else if (role == ROLE_LINK_STATE ) {
        quint32 s = l->getLinkState();
        if( GenComLink::CONNECTION_COMMAND_START == l->getCurrentCommand() && s!= GenComLink::LINK_STATE_CONNECTED) {
            s = GenComLink::LINK_STATE_CONNECTING;
        }
        ret = s;
    } else if (role == ROLE_SERIAL_PORT ) {
        ComLinkSerial * ls = l->getLinkSerial() ;
        if( nullptr != ls ) {
            if( 0 != ls->getPortNumber() ) {
                ret = "COM"+QString::number(ls->getPortNumber());
            } else {
                ret = "UNDEF";
            }
        }
    } else if (role == ROLE_SERIAL_PORT_LIST ) {
        ComLinkSerial * ls = l->getLinkSerial() ;
        if( nullptr != ls ) {
            QStringList sl = m_pComLinkManager->comPortList();
            if( sl.contains("COM"+QString::number(ls->getPortNumber()))) {
                sl.removeAll("COM"+QString::number(ls->getPortNumber()));
            }
            sl.insert(0,"COM"+QString::number(ls->getPortNumber()));
            sl.removeAll("COM0");
            if( !ls->isConnected() && !sl.contains("UNDEF") ) {
                int indexInsert = sl.size() ? 1 : 0;
                sl.insert(indexInsert,"UNDEF");
            }
            ret = sl;
        }
    } else if (role == ROLE_SERIAL_BAUDRATE ) {
        ComLinkSerial * ls = l->getLinkSerial() ;
        if( nullptr != ls ) {
            ret = QString::number(ls->getBaudrate());
        }
    } else if (role == ROLE_IP_ADDRESS ) {
        ComLinkTcpCli * ls = l->getLinkTcpCli();
        if( nullptr != ls ) {
            // TODO
        }
    } else if (role == ROLE_IP_PORT ) {
        ComLinkTcpCli * ls = l->getLinkTcpCli();
        if( nullptr != ls ) {
            // TODO
        }
    }

    return ret;
}

QHash<int, QByteArray> ComLinkModel::roleNames() const
{
    QHash <int,QByteArray> roles = QAbstractListModel::roleNames();
    roles[ROLE_LINK_TYPE] = "link_type";
    roles[ROLE_LINK_NAME] = "link_name";
    roles[ROLE_LINK_STATE] = "link_state";
    roles[ROLE_SERIAL_PORT] = "serial_port";
    roles[ROLE_SERIAL_PORT_LIST] = "serial_port_list";
    roles[ROLE_SERIAL_BAUDRATE] = "serial_baudrate";
    roles[ROLE_IP_ADDRESS] = "ip_address";
    roles[ROLE_IP_PORT] = "ip_port";

    return roles;
}

void ComLinkModel::addLink()
{
    beginInsertRows( QModelIndex(), 0, 0); //m_plLink->size()-1, m_plLink->size());
    endInsertRows();
}

void ComLinkModel::removeLink(int p_row)
{
    beginRemoveRows( QModelIndex(), p_row, p_row);
    endRemoveRows();
}


void ComLinkModel::resetModel()
{
    beginRemoveRows( QModelIndex(), 0, m_plLink->size());
    endRemoveRows();
}

void ComLinkModel::updateLinks()
{    
    if( m_plLink->size() ) {
        emit dataChanged(createIndex(0,0), createIndex(m_plLink->size()-1, 0));
    } else {
        emit dataChanged(createIndex(0,0), createIndex(0, 0));
    }
}
