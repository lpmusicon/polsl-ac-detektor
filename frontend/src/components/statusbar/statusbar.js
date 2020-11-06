import styled from "styled-components";

const StatusbarTitle = styled.h2`
  flex: 1;
  color: #becbd8;
  font-size: 24px;
  font-weight: normal;
  margin: 0;
  padding: 0;
  text-align: center;
`;

const StatusbarStyled = styled.div`
  display: flex;
  flex-direction: row;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 8px;
`;

const Statusbar = ({ left, title, right }) => {
  return (
    <StatusbarStyled>
      {left}
      <StatusbarTitle>{title}</StatusbarTitle>
      {right}
    </StatusbarStyled>
  );
};

export default Statusbar;
